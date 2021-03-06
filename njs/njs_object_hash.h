
/*
 * Copyright (C) Igor Sysoev
 * Copyright (C) NGINX, Inc.
 */

#ifndef _NJS_OBJECT_HASH_H_INCLUDED_
#define _NJS_OBJECT_HASH_H_INCLUDED_


#define NJS_CONFIGURABLE_HASH                                                 \
    nxt_djb_hash_add(                                                         \
    nxt_djb_hash_add(                                                         \
    nxt_djb_hash_add(                                                         \
    nxt_djb_hash_add(                                                         \
    nxt_djb_hash_add(                                                         \
    nxt_djb_hash_add(                                                         \
    nxt_djb_hash_add(                                                         \
    nxt_djb_hash_add(                                                         \
    nxt_djb_hash_add(                                                         \
    nxt_djb_hash_add(                                                         \
    nxt_djb_hash_add(                                                         \
    nxt_djb_hash_add(NXT_DJB_HASH_INIT,                                       \
        'c'), 'o'), 'n'), 'f'), 'i'), 'g'), 'u'), 'r'), 'a'), 'b'), 'l'), 'e')


#define NJS_CONSTRUCTOR_HASH                                                  \
    nxt_djb_hash_add(                                                         \
    nxt_djb_hash_add(                                                         \
    nxt_djb_hash_add(                                                         \
    nxt_djb_hash_add(                                                         \
    nxt_djb_hash_add(                                                         \
    nxt_djb_hash_add(                                                         \
    nxt_djb_hash_add(                                                         \
    nxt_djb_hash_add(                                                         \
    nxt_djb_hash_add(                                                         \
    nxt_djb_hash_add(                                                         \
    nxt_djb_hash_add(NXT_DJB_HASH_INIT,                                       \
        'c'), 'o'), 'n'), 's'), 't'), 'r'), 'u'), 'c'), 't'), 'o'), 'r')


#define NJS_ENUMERABLE_HASH                                                   \
    nxt_djb_hash_add(                                                         \
    nxt_djb_hash_add(                                                         \
    nxt_djb_hash_add(                                                         \
    nxt_djb_hash_add(                                                         \
    nxt_djb_hash_add(                                                         \
    nxt_djb_hash_add(                                                         \
    nxt_djb_hash_add(                                                         \
    nxt_djb_hash_add(                                                         \
    nxt_djb_hash_add(                                                         \
    nxt_djb_hash_add(NXT_DJB_HASH_INIT,                                       \
        'e'), 'n'), 'u'), 'm'), 'e'), 'r'), 'a'), 'b'), 'l'), 'e')


#define NJS_INDEX_HASH                                                        \
    nxt_djb_hash_add(                                                         \
    nxt_djb_hash_add(                                                         \
    nxt_djb_hash_add(                                                         \
    nxt_djb_hash_add(                                                         \
    nxt_djb_hash_add(NXT_DJB_HASH_INIT,                                       \
        'i'), 'n'), 'd'), 'e'), 'x')


#define NJS_INPUT_HASH                                                        \
    nxt_djb_hash_add(                                                         \
    nxt_djb_hash_add(                                                         \
    nxt_djb_hash_add(                                                         \
    nxt_djb_hash_add(                                                         \
    nxt_djb_hash_add(NXT_DJB_HASH_INIT,                                       \
        'i'), 'n'), 'p'), 'u'), 't')


#define NJS_JOIN_HASH                                                         \
    nxt_djb_hash_add(                                                         \
    nxt_djb_hash_add(                                                         \
    nxt_djb_hash_add(                                                         \
    nxt_djb_hash_add(NXT_DJB_HASH_INIT,                                       \
        'j'), 'o'), 'i'), 'n')


#define NJS_PROTOTYPE_HASH                                                    \
    nxt_djb_hash_add(                                                         \
    nxt_djb_hash_add(                                                         \
    nxt_djb_hash_add(                                                         \
    nxt_djb_hash_add(                                                         \
    nxt_djb_hash_add(                                                         \
    nxt_djb_hash_add(                                                         \
    nxt_djb_hash_add(                                                         \
    nxt_djb_hash_add(                                                         \
    nxt_djb_hash_add(NXT_DJB_HASH_INIT,                                       \
        'p'), 'r'), 'o'), 't'), 'o'), 't'), 'y'), 'p'), 'e')


#define NJS_TO_JSON_HASH                                                      \
    nxt_djb_hash_add(                                                         \
    nxt_djb_hash_add(                                                         \
    nxt_djb_hash_add(                                                         \
    nxt_djb_hash_add(                                                         \
    nxt_djb_hash_add(                                                         \
    nxt_djb_hash_add(NXT_DJB_HASH_INIT,                                       \
        't'), 'o'), 'J'), 'S'), 'O'), 'N')


#define NJS_TO_STRING_HASH                                                    \
    nxt_djb_hash_add(                                                         \
    nxt_djb_hash_add(                                                         \
    nxt_djb_hash_add(                                                         \
    nxt_djb_hash_add(                                                         \
    nxt_djb_hash_add(                                                         \
    nxt_djb_hash_add(                                                         \
    nxt_djb_hash_add(                                                         \
    nxt_djb_hash_add(NXT_DJB_HASH_INIT,                                       \
        't'), 'o'), 'S'), 't'), 'r'), 'i'), 'n'), 'g')


#define NJS_VALUE_HASH                                                        \
    nxt_djb_hash_add(                                                         \
    nxt_djb_hash_add(                                                         \
    nxt_djb_hash_add(                                                         \
    nxt_djb_hash_add(                                                         \
    nxt_djb_hash_add(NXT_DJB_HASH_INIT,                                       \
        'v'), 'a'), 'l'), 'u'), 'e')


#define NJS_VALUE_OF_HASH                                                     \
    nxt_djb_hash_add(                                                         \
    nxt_djb_hash_add(                                                         \
    nxt_djb_hash_add(                                                         \
    nxt_djb_hash_add(                                                         \
    nxt_djb_hash_add(                                                         \
    nxt_djb_hash_add(                                                         \
    nxt_djb_hash_add(NXT_DJB_HASH_INIT,                                       \
        'v'), 'a'), 'l'), 'u'), 'e'), 'O'), 'f')


#define NJS_TO_ISO_STRING_HASH                                                \
    nxt_djb_hash_add(                                                         \
    nxt_djb_hash_add(                                                         \
    nxt_djb_hash_add(                                                         \
    nxt_djb_hash_add(                                                         \
    nxt_djb_hash_add(                                                         \
    nxt_djb_hash_add(                                                         \
    nxt_djb_hash_add(                                                         \
    nxt_djb_hash_add(                                                         \
    nxt_djb_hash_add(                                                         \
    nxt_djb_hash_add(                                                         \
    nxt_djb_hash_add(NXT_DJB_HASH_INIT,                                       \
        't'), 'o'), 'I'), 'S'), 'O'), 'S'), 't'), 'r'), 'i'), 'n'), 'g')


#define NJS_WRITABABLE_HASH                                                   \
    nxt_djb_hash_add(                                                         \
    nxt_djb_hash_add(                                                         \
    nxt_djb_hash_add(                                                         \
    nxt_djb_hash_add(                                                         \
    nxt_djb_hash_add(                                                         \
    nxt_djb_hash_add(                                                         \
    nxt_djb_hash_add(                                                         \
    nxt_djb_hash_add(NXT_DJB_HASH_INIT,                                       \
        'w'), 'r'), 'i'), 't'), 'a'), 'b'), 'l'), 'e')


#endif /* _NJS_OBJECT_HASH_H_INCLUDED_ */
