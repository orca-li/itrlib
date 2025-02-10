#ifndef DDS_GUID32_BAA40661
#define DDS_GUID32_BAA40661

/**
 * DDS TYPES
 * @warning this is index
 */
typedef enum _DDS_TYPES
{
    DDS_TYPE_LIST = 0,
    DDS_TYPE_RING = 1,
    DDS_TYPE_ARRAY,
    DDS_TYPE_TREE,
    DDS_TYPE_BST,
    DDS_TYPE_HEAP,
    DDS_TYPE_STACK,
    DDS_TYPE_QUEUE,
    DDS_TYPE_DEQUEUE,
    DDS_TYPE_JSON,
    DDS_TYPE_HASH,
    DDS_TYPE_SET,
    DDS_TYPE_GRAPH,
} DDS_TYPES;

typedef enum _DDS_INSERT_TYPES
{
    DDS_INS_TO_HEAD,
    DDS_INS_TO_TAIL,
    DDS_INS_DATA,
    DDS_INS_REPLACE,
    DDS_INS_NEXT,
    DDS_INS_PREV,
} DDS_INSERT_TYPES;

typedef enum _DDS_GET_REQUESTS
{
    DDS_GET_DATA,
    DDS_GET_NEXT_DATA,
    DDS_GET_PREV_DATA,
    DDS_GET_TAIL_DATA,
    DDS_GET_HEAD_DATA,
    DDS_GET_NEXT_DDS,
    DDS_GET_PREV_DDS,
    DDS_GET_TAIL_DDS,
    DDS_GET_HEAD_DDS,
} DDS_GET_REQUESTS;

typedef enum _DDS_DETACH_REQUESTS
{
    DDS_DETACH_THIS,
    DDS_DETACH_NEXT,
} DDS_DETACH_REQUESTS;

#define DDS_CHECK_MASK(_var, _mask) ((_var & _mask) == _mask)

typedef unsigned int ddsint_t;
typedef unsigned int ddsflags_t;
typedef unsigned char ddsbool_t;

#endif /* DDS_GUID32_BAA40661 */