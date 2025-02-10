#ifdef ITRLIB_DEBUG
#include "../include/itrdef.h"
#include "../include/itrlog.h"
#include "../include/itrobj.h"

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

#include <ddslib.h>

#define STR_FILENAME "ITR_OUTPUT.log"
#define STR_NONE "NONE"
#define STR_DEBUG "DEBUG"
#define STR_INFO "INFO"
#define STR_WARNING "WARNING"
#define STR_TRACE "TRACE"
#define STR_STEP "STEP"
#define STR_DUMP "DUMP"
#define STR_CALLBACK "CALLBACK"
#define STR_ERROR "ERROR"

#define NUMBOF_OUTPUT 2
#define NUMBOF_NULL 1

#define LVLBUF_SIZE 16
#define OUTPUT_LIST_SIZE (NUMBOF_OUTPUT + NUMBOF_NULL)
#define BUFFER_SIZE 1024

typedef void (*LogHandler_t)(char *, ...);

static void _FileIoHandler(char* str);
static void RemoveFileHandler(char* str);
static void LogTransmit(char *str);
static void LogPrintf(itrlog_lvl_t lvl, const char *file, const char *func, int line, char *fmt, va_list args);
static void LoadLogLevelsHandler(itrlog_lvl_t lvl, const char *file, const char *func, int line, char *fmt, va_list args);

static itrlog_lvl_t CurrentLevelConfig = ITR_L_DUMP | ITR_L_STDOUT | ITR_L_FILE | ITR_L_ERROR | ITR_L_TRACE | ITR_L_WARNING | ITR_L_DEBUG | ITR_L_INFO;

static void (*itr_fwrite)(char*) = RemoveFileHandler;
static void (*SwitchLogPrint)(itrlog_lvl_t lvl, const char *file, const char *func, int line, char *fmt, va_list args) = LoadLogLevelsHandler;
static void (*OutputList[OUTPUT_LIST_SIZE])(char *str) = {NULL};

static itrbool_t isVarOverflow(int c, int limit)
{
    if (c >= 0 && c < limit)
        return ITR_FALSE;

    LogTransmit("Buffer overflow!\r\n");
    return ITR_TRUE;
}

static void RemoveFileHandler(char* str)
{
    if (ITR_CHECK_MASK(CurrentLevelConfig, ITR_L_FILE))
        remove(STR_FILENAME);

    itr_fwrite = _FileIoHandler;
    itr_fwrite(str);
}

static void _FileIoHandler(char* str)
{
    FILE *handler;
    handler = fopen(STR_FILENAME, "a");
    if (!handler)
        return;

    fwrite(str, 1, strlen(str), handler);
    fclose(handler);
}

static void FileIoHandler(char *str)
{
    itr_fwrite(str);
}

static void StdioHandler(char *str)
{
    fprintf(stdout, "%s", str);
}

void __ITR_DBG_CONFIG__(itrlog_lvl_t enable, itrlog_lvl_t disable, ...)
{
    CurrentLevelConfig |= enable;
    CurrentLevelConfig &= ~disable;
}

static void LoadLogLevelsHandler(itrlog_lvl_t lvl, const char *file, const char *func, int line, char *fmt, va_list args)
{
    itrint_t outidx = 0;
    if (ITR_CHECK_MASK(CurrentLevelConfig, ITR_L_FILE))
    {
        OutputList[outidx] = FileIoHandler;
        outidx++;
    }

    if (ITR_CHECK_MASK(CurrentLevelConfig, ITR_L_STDOUT))
    {
        OutputList[outidx] = StdioHandler;
        outidx++;
    }

    OutputList[outidx] = NULL;

    SwitchLogPrint = LogPrintf;
    SwitchLogPrint(lvl, file, func, line, fmt, args);
}

static void LogTransmit(char *str)
{
    itrint_t i;

    for (i = 0; i < OUTPUT_LIST_SIZE; i++)
    {
        if (OutputList[i] != NULL)
            OutputList[i](str);
        else
            return;
    }
}

static void LevelToString(char *buffer, itrlog_lvl_t lvl)
{
    const char *STR_LEVEL;

    switch (lvl)
    {
    case ITR_L_INFO:
        STR_LEVEL = STR_INFO;
        break;

    case ITR_L_DEBUG:
        STR_LEVEL = STR_DEBUG;
        break;

    case ITR_L_WARNING:
        STR_LEVEL = STR_WARNING;
        break;

    case ITR_L_TRACE:
        STR_LEVEL = STR_TRACE;
        break;

    case ITR_L_DUMP:
        STR_LEVEL = STR_DUMP;
        break;

    case ITR_L_STEP:
        STR_LEVEL = STR_STEP;
        break;

    case ITR_L_ERROR:
        STR_LEVEL = STR_ERROR;
        break;

    case ITR_L_CALLBACK:
        STR_LEVEL =  STR_CALLBACK;
        break;

    default:
        STR_LEVEL = STR_NONE;
        break;
    }

    sprintf(buffer, "%s", STR_LEVEL);
}

static void DumpLevelHandler(itrobj_t this, char* base)
{
    if (!this)
        return;
    char* fillptr;

    fillptr = base;
    fillptr += sprintf(fillptr, "  >> [ID] autoid: %d\r\n", this->autoid);
    fillptr += sprintf(fillptr, "  >> [LOCATION] base:%p locate:%p\r\n", this->base, this->locate);
    fillptr += sprintf(fillptr, "  >> [COUNT] stepsz:%d limite:%d index:%d\r\n", this->stepsz, this->limite, this->index);

    fillptr += sprintf(fillptr, "  >> [KIN] parent:%p child:%p\r\n", ddsget(this->kin, DDS_GET_PREV_DDS), ddsget(this->kin, DDS_GET_NEXT_DDS));
    fillptr += sprintf(fillptr, "  >> [PEERS] PREV:%p NEXT:%p\r\n", ddsget(this->peer, DDS_GET_PREV_DDS), ddsget(this->peer, DDS_GET_NEXT_DDS));

    LogTransmit(base);
}
static void LogPrintf(itrlog_lvl_t lvl, const char *file, const char *func, int line, char *fmt, va_list args)
{
    ITR_STATIC char msgbuf[BUFFER_SIZE];
    ITR_STATIC char buffer[BUFFER_SIZE];
    ITR_STATIC char lvlstr[LVLBUF_SIZE];
    itrobj_t this;
    int c;

    if (!ITR_CHECK_MASK(CurrentLevelConfig, lvl))
        return;

    c = vsnprintf(msgbuf, BUFFER_SIZE, fmt, args);
    if (isVarOverflow(c, BUFFER_SIZE))
        return;

    if (lvl == ITR_L_DUMP)
        this = va_arg(args, itrobj_t);

    LevelToString(lvlstr, lvl);
    c = snprintf(buffer, BUFFER_SIZE, "[%s] %s(%s:%d): %s\r\n", lvlstr, file, func, line, msgbuf);
    if (isVarOverflow(c, BUFFER_SIZE))
        return;

    LogTransmit(buffer);
    if (lvl == ITR_L_DUMP)
        DumpLevelHandler(this, buffer);
}

void __ITRLOG__(itrlog_lvl_t lvl, const char *file, const char *func, int line, char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);

    SwitchLogPrint(lvl, file, func, line, fmt, args);

    va_end(args);
}

#endif /* ifdef ITRLIB_DEBUG */