#ifndef ITR_GUID32_CD2BA7CB
#define ITR_GUID32_CD2BA7CB

#ifndef ITRLIB_DISABLE_SECURITY_MODE
#define ITR_STATIC static
#else
#define ITR_STATIC
#endif
#define ITR_TRUE            ((itrbool_t)1==1)
#define ITR_FALSE           ((itrbool_t)1==0)

#define ITR_SUCCESS         0x00000000u
#define ITR_ERROR           0x00000001u

/**
 * LINKS/UNLINKS TYPE
 */
#define ITR_PEER_TO_PEER    0x00000001u
#define ITR_PARENT_CHILD    0x00000002u
#define ITR_PARENT          0x00000004u
#define ITR_CHILD           0x00000008u
#define ITR_PEER            0x00000010u

/**
 * ITERATOR MODIFIERS
 */
#define ITR_LIMITATION      0x00000001u

/**
 * CALLBACKS TYPE
 */
#define ITR_STEP            0x00000001u
#define ITR_START           0x00000002u
#define ITR_OVERFLOW        0x00000004u
#define ITR_STOP            0x00000008u
#define ITR_RESET           0x00000010u

/**
 * COMPARE
 */
#define ITR_DATA            0x00000001u
#define ITR_MEMORY          0x00000002u
#define ITR_INDEX           0x00000004u
#define ITR_LOCATE          0x00000008u
#define ITR_COMPARE_ERROR   0xFFFFFFFFu

#define ITR_CHECK_MASK(_var, _mask) ((_var & _mask) == _mask)

typedef unsigned int itrint_t;
typedef char* itrptr_t;
typedef unsigned short itrid_t;
typedef unsigned int itrflags_t;
typedef unsigned char itrbool_t;
typedef unsigned char itrstatus_t;

#ifdef ITRLIB_ENABLE_ASSERT
#define ITR_ASSERT(...) assert(__VA_ARGS__)
#else
#define ITR_ASSERT()
#endif

#endif /* ITR_GUID32_CD2BA7CB */