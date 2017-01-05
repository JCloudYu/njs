
/*
 * Copyright (C) Igor Sysoev
 * Copyright (C) NGINX, Inc.
 */

#include <nxt_auto_config.h>
#include <nxt_types.h>
#include <nxt_clang.h>
#include <nxt_alignment.h>
#include <nxt_string.h>
#include <nxt_stub.h>
#include <nxt_array.h>
#include <nxt_lvlhsh.h>
#include <nxt_random.h>
#include <nxt_malloc.h>
#include <nxt_mem_cache_pool.h>
#include <njscript.h>
#include <njs_vm.h>
#include <njs_string.h>
#include <njs_object.h>
#include <njs_function.h>
#include <njs_variable.h>
#include <njs_parser.h>
#include <njs_regexp.h>
#include <string.h>


static void *
njs_alloc(void *mem, size_t size)
{
    return nxt_malloc(size);
}


static void *
njs_zalloc(void *mem, size_t size)
{
    void  *p;

    p = nxt_malloc(size);

    if (p != NULL) {
        memset(p, 0, size);
    }

    return p;
}


static void *
njs_align(void *mem, size_t alignment, size_t size)
{
    return nxt_memalign(alignment, size);
}


static void
njs_free(void *mem, void *p)
{
    nxt_free(p);
}


static const nxt_mem_proto_t  njs_vm_mem_cache_pool_proto = {
    njs_alloc,
    njs_zalloc,
    njs_align,
    NULL,
    njs_free,
    NULL,
    NULL,
};


static void *
njs_array_mem_alloc(void *mem, size_t size)
{
    return nxt_mem_cache_alloc(mem, size);
}


static void
njs_array_mem_free(void *mem, void *p)
{
    nxt_mem_cache_free(mem, p);
}


const nxt_mem_proto_t  njs_array_mem_proto = {
    njs_array_mem_alloc,
    NULL,
    NULL,
    NULL,
    njs_array_mem_free,
    NULL,
    NULL,
};


njs_vm_t *
njs_vm_create(nxt_mem_cache_pool_t *mcp, njs_vm_shared_t **shared,
    nxt_lvlhsh_t *externals)
{
    njs_vm_t              *vm;
    nxt_int_t             ret;
    njs_regexp_pattern_t  *pattern;

    if (mcp == NULL) {
        mcp = nxt_mem_cache_pool_create(&njs_vm_mem_cache_pool_proto, NULL,
                                        NULL, 2 * nxt_pagesize(), 128, 512, 16);
        if (nxt_slow_path(mcp == NULL)) {
            return NULL;
        }
    }

    vm = nxt_mem_cache_zalign(mcp, sizeof(njs_value_t), sizeof(njs_vm_t));

    if (nxt_fast_path(vm != NULL)) {
        vm->mem_cache_pool = mcp;

        ret = njs_regexp_init(vm);
        if (nxt_slow_path(ret != NXT_OK)) {
            return NULL;
        }

        if (shared != NULL && *shared != NULL) {
            vm->shared = *shared;

        } else {
            vm->shared = nxt_mem_cache_zalloc(mcp, sizeof(njs_vm_shared_t));
            if (nxt_slow_path(vm->shared == NULL)) {
                return NULL;
            }

            if (shared != NULL) {
                *shared = vm->shared;
            }

            nxt_lvlhsh_init(&vm->shared->keywords_hash);

            ret = njs_lexer_keywords_init(mcp, &vm->shared->keywords_hash);
            if (nxt_slow_path(ret != NXT_OK)) {
                return NULL;
            }

            nxt_lvlhsh_init(&vm->shared->values_hash);

            pattern = njs_regexp_pattern_create(vm, (u_char *) "(?:)",
                                                sizeof("(?:)") - 1, 0);
            if (nxt_slow_path(pattern == NULL)) {
                return NULL;
            }

            vm->shared->empty_regexp_pattern = pattern;

            ret = njs_builtin_objects_create(vm);
            if (nxt_slow_path(ret != NXT_OK)) {
                return NULL;
            }
        }

        nxt_lvlhsh_init(&vm->values_hash);

        if (externals != NULL) {
            vm->externals_hash = *externals;
        }

        vm->trace.level = NXT_LEVEL_TRACE;
        vm->trace.size = 2048;
        vm->trace.handler = njs_parser_trace_handler;
        vm->trace.data = vm;
    }

    return vm;
}


void
njs_vm_destroy(njs_vm_t *vm)
{
    nxt_mem_cache_pool_destroy(vm->mem_cache_pool);
}


nxt_int_t
njs_vm_compile(njs_vm_t *vm, u_char **start, u_char *end,
    njs_function_t **function, nxt_str_t **export)
{
    nxt_int_t          ret;
    njs_lexer_t        *lexer;
    njs_parser_t       *parser;
    njs_variable_t     *var;
    njs_parser_node_t  *node;

    parser = nxt_mem_cache_zalloc(vm->mem_cache_pool, sizeof(njs_parser_t));
    if (nxt_slow_path(parser == NULL)) {
        return NJS_ERROR;
    }

    vm->parser = parser;

    lexer = nxt_mem_cache_zalloc(vm->mem_cache_pool, sizeof(njs_lexer_t));
    if (nxt_slow_path(lexer == NULL)) {
        return NJS_ERROR;
    }

    parser->lexer = lexer;
    lexer->start = *start;
    lexer->end = end;
    lexer->line = 1;
    lexer->keywords_hash = vm->shared->keywords_hash;

    parser->code_size = sizeof(njs_vmcode_stop_t);
    parser->scope_offset = NJS_INDEX_GLOBAL_OFFSET;

    node = njs_parser(vm, parser);
    if (nxt_slow_path(node == NULL)) {
        return NJS_ERROR;
    }

    if (function != NULL) {
        if (node->token == NJS_TOKEN_CALL) {
            var = njs_variable_get(vm, node->right, NJS_NAME_DECLARATION);
            if (nxt_slow_path(var == NULL)) {
                return NJS_ERROR;
            }

            *function = var->value.data.u.function;

        } else {
            *function = NULL;
        }
    }

    *start = parser->lexer->start;

    ret = njs_generate_scope(vm, parser, node);
    if (nxt_slow_path(ret != NXT_OK)) {
        return NJS_ERROR;
    }

    vm->current = parser->code_start;

    vm->global_scope = parser->local_scope;
    vm->scope_size = parser->scope_size;
    vm->variables_hash = parser->scope->variables;

    vm->parser = NULL;

    *export = njs_vm_export_functions(vm);
    if (nxt_slow_path(*export == NULL)) {
        return NJS_ERROR;
    }

    return NJS_OK;
}


njs_vm_t *
njs_vm_clone(njs_vm_t *vm, nxt_mem_cache_pool_t *mcp, void **external)
{
    u_char                *values;
    size_t                size, scope_size;
    njs_vm_t              *nvm;
    nxt_int_t             ret;
    njs_frame_t           *frame;
    nxt_mem_cache_pool_t  *nmcp;

    nxt_thread_log_debug("CLONE:");

    nmcp = mcp;

    if (nmcp == NULL) {
        nmcp = nxt_mem_cache_pool_create(&njs_vm_mem_cache_pool_proto, NULL,
                                        NULL, 2 * nxt_pagesize(), 128, 512, 16);
        if (nxt_slow_path(nmcp == NULL)) {
            return NULL;
        }
    }

    nvm = nxt_mem_cache_zalloc(nmcp, sizeof(njs_vm_t));

    if (nxt_fast_path(nvm != NULL)) {
        nvm->mem_cache_pool = nmcp;

        nvm->shared = vm->shared;

        nvm->variables_hash = vm->variables_hash;
        nvm->values_hash = vm->values_hash;
        nvm->externals_hash = vm->externals_hash;

        nvm->retval = njs_value_void;
        nvm->current = vm->current;
        nvm->external = external;

        nvm->global_scope = vm->global_scope;
        scope_size = vm->scope_size;
        nvm->scope_size = scope_size;
        scope_size += NJS_INDEX_GLOBAL_OFFSET;

        size = NJS_GLOBAL_FRAME_SIZE + scope_size + NJS_FRAME_SPARE_SIZE;
        size = nxt_align_size(size, NJS_FRAME_SPARE_SIZE);

        frame = nxt_mem_cache_align(nmcp, sizeof(njs_value_t), size);
        if (nxt_slow_path(frame == NULL)) {
            goto fail;
        }

        memset(frame, 0, NJS_GLOBAL_FRAME_SIZE);

        nvm->frame = &frame->native;

        frame->native.size = size;
        frame->native.free_size = size - (NJS_GLOBAL_FRAME_SIZE + scope_size);

        values = (u_char *) frame + NJS_GLOBAL_FRAME_SIZE;

        frame->native.free = values + scope_size;

        nvm->scopes[NJS_SCOPE_GLOBAL] = (njs_value_t *) values;
        memcpy(values + NJS_INDEX_GLOBAL_OFFSET, vm->global_scope,
               vm->scope_size);

        ret = njs_regexp_init(nvm);
        if (nxt_slow_path(ret != NXT_OK)) {
            goto fail;
        }

        ret = njs_builtin_objects_clone(nvm);
        if (nxt_slow_path(ret != NXT_OK)) {
            goto fail;
        }

        nvm->trace.level = NXT_LEVEL_TRACE;
        nvm->trace.size = 2048;
        nvm->trace.handler = njs_parser_trace_handler;
        nvm->trace.data = nvm;

        return nvm;
    }

fail:

    if (mcp == NULL) {
        nxt_mem_cache_pool_destroy(nmcp);
    }

    return NULL;
}


nxt_int_t
njs_vm_call(njs_vm_t *vm, njs_function_t *function, njs_opaque_value_t *args,
    nxt_uint_t nargs)
{
    u_char       *current;
    njs_ret_t    ret;
    njs_value_t  *this;

    static const njs_vmcode_stop_t  stop[] = {
        { .code = { .operation = njs_vmcode_stop,
                    .operands =  NJS_VMCODE_1OPERAND,
                    .retval = NJS_VMCODE_NO_RETVAL },
          .retval = NJS_INDEX_GLOBAL_RETVAL },
    };

    this = (njs_value_t *) &njs_value_void;

    ret = njs_function_frame(vm, function, this,
                             (njs_value_t *) args, nargs, 0);

    if (nxt_slow_path(ret != NXT_OK)) {
        return ret;
    }

    current = vm->current;
    vm->current = (u_char *) stop;

    (void) njs_function_call(vm, NJS_INDEX_GLOBAL_RETVAL, 0);

    ret = njs_vmcode_interpreter(vm);

    vm->current = current;

    if (ret == NJS_STOP) {
        ret = NXT_OK;
    }

    return ret;
}


nxt_int_t
njs_vm_run(njs_vm_t *vm)
{
    nxt_str_t  s;
    nxt_int_t  ret;

    nxt_thread_log_debug("RUN:");

    ret = njs_vmcode_interpreter(vm);

    if (nxt_slow_path(ret == NXT_AGAIN)) {
        nxt_thread_log_debug("VM: AGAIN");
        return ret;
    }

    if (nxt_slow_path(ret != NJS_STOP)) {
        nxt_thread_log_debug("VM: ERROR");
        return ret;
    }

    if (vm->retval.type == NJS_NUMBER) {
        nxt_thread_log_debug("VM: %f", vm->retval.data.u.number);

    } else if (vm->retval.type == NJS_BOOLEAN) {
        nxt_thread_log_debug("VM: boolean: %d", vm->retval.data.truth);

    } else if (vm->retval.type == NJS_STRING) {

        if (njs_value_to_ext_string(vm, &s, &vm->retval) == NJS_OK) {
            nxt_thread_log_debug("VM: '%V'", &s);
        }

    } else if (vm->retval.type == NJS_FUNCTION) {
        nxt_thread_log_debug("VM: function");

    } else if (vm->retval.type == NJS_NULL) {
        nxt_thread_log_debug("VM: null");

    } else if (vm->retval.type == NJS_VOID) {
        nxt_thread_log_debug("VM: void");

    } else {
        nxt_thread_log_debug("VM: unknown: %d", vm->retval.type);
    }

    return NJS_OK;
}


njs_ret_t
njs_vm_return_string(njs_vm_t *vm, u_char *start, size_t size)
{
    return njs_string_create(vm, &vm->retval, start, size, 0);
}


nxt_int_t
njs_vm_retval(njs_vm_t *vm, nxt_str_t *retval)
{
    return njs_value_to_ext_string(vm, retval, &vm->retval);
}


nxt_int_t
njs_vm_exception(njs_vm_t *vm, nxt_str_t *retval)
{
    return njs_value_to_ext_string(vm, retval, vm->exception);
}
