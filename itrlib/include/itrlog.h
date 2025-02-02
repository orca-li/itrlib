#ifndef ITR_GUID32_D073DB97
#define ITR_GUID32_D073DB97

#define ITRLOG_LVL_INFO         0x000001
#define ITRLOG_LVL_DEBUG        0x000002
#define ITRLOG_LVL_WARNING      0x000004
#define ITRLOG_LVL_TRACE        0x000008
#define ITRLOG_LVL_DUMP         0x000010
#define ITRLOG_LVL_ERROR        0x000080
#define ITRLOG_GRP_STEP         0x000100
#define ITRLOG_GRP_CALLBACK     0x000200
#define ITRLOG_OUT_FILE         0x010000
#define ITRLOG_OUT_STDOUT       0x020000

#define ITR_L_INFO ITRLOG_LVL_INFO
#define ITR_L_DEBUG ITRLOG_LVL_DEBUG
#define ITR_L_WARNING ITRLOG_LVL_WARNING
#define ITR_L_TRACE ITRLOG_LVL_TRACE
#define ITR_L_DUMP ITRLOG_LVL_DUMP
#define ITR_L_ERROR ITRLOG_LVL_ERROR
#define ITR_L_STEP ITRLOG_GRP_STEP
#define ITR_L_CALLBACK ITRLOG_GRP_CALLBACK
#define ITR_L_FILE ITRLOG_OUT_FILE
#define ITR_L_STDOUT ITRLOG_OUT_STDOUT

#ifdef ITRLIB_DEBUG

#define __ITRLOG(_lvl, ...) __ITRLOG__(_lvl, __VA_ARGS__)
#define _ITRLOG(_lvl, ...) __ITRLOG(_lvl, __FILE__, __func__, __LINE__, __VA_ARGS__)

typedef unsigned int itrlog_lvl_t;

#define __ITR_DBG_CONFIG(...) __ITR_DBG_CONFIG__(__VA_ARGS__)
#define _ITR_DBG_CONFIG(...) __ITR_DBG_CONFIG(__VA_ARGS__)
#define ITR_DBG_CONFIG(...) _ITR_DBG_CONFIG(__VA_ARGS__)

void __ITR_DBG_CONFIG__(itrlog_lvl_t enable, itrlog_lvl_t disable, ...);
void __ITRLOG__(itrlog_lvl_t lvl, const char* file, const char* func, int line,  char *fmt, ...);

#else /* ITRLIB_DEBUG*/
#define _ITRLOG(...)
#endif /* ITRLIB_DEBUG*/

#define ITRLOG(...) _ITRLOG(__VA_ARGS__)

#endif /* ITR_GUID32_D073DB97 */