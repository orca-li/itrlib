#include <itrlib.h>
#include <stdio.h>
#include <string.h>
#include <ddslib.h>
#include <stdarg.h>

#define SIZESTR(_str) (strlen(_str) + 1)
#define STR_BYE "Goodbye!\r\n"
#define STR_GONE "Gone! 0_o\r\n"


#define STR_P1 "PEER 1"
#define STR_P2 "PEER 2"
#define STR_P3 "PEER 3"

#ifdef DDS_MAIN
#define STR_HELLO "Hello, world!\r\n"

int main()
{
    ddsobj_t p1, p2, p3;
    char* str;

    p1 = ddscnst(DDS_TYPE_LIST);
    p2 = ddscnst(DDS_TYPE_LIST);
    p3 = ddscnst(DDS_TYPE_LIST);

    ddsins(p1, DDS_INS_DATA, STR_P1, SIZESTR(STR_P1));
    ddsins(p2, DDS_INS_DATA, STR_P2, SIZESTR(STR_P2));
    ddsins(p3, DDS_INS_DATA, STR_P3, SIZESTR(STR_P3));

    ddsins(p1, DDS_INS_NEXT, p2);
    ddsins(p2, DDS_INS_NEXT, p3);

    str = ddsget(p3, DDS_GET_HEAD_DATA);
    printf("%s ", str);
    str = ddsget(p3, DDS_GET_PREV_DATA);
    printf("%s ", str);
    str = ddsget(p3, DDS_GET_DATA);
    printf("%s\n", str);

    ddsdetach(p2, DDS_DETACH_THIS);

    str = ddsget(p3, DDS_GET_HEAD_DATA);
    printf("%s ", str);
    str = ddsget(p1, DDS_GET_NEXT_DATA);
    printf("%s\n", str);

    ddsdstr(p1);
    ddsdstr(p2);
    ddsdstr(p3);

    return 0;
}

#else

#define STR_HELLO "Hello, world!"
#define IDX_HELLO 0x22

#define IDX_ITR1_ID 0x35
#define IDX_ITR2_ID 0x15

#define REGION_SIZE 64
#define REGION_LENGTH REGION_SIZE - 1

static char region[REGION_SIZE];

static void filling(itrobj_t this, ...)
{
    static unsigned char value = 0;

    *itrread(this) = value++;
}

static void dump(itrobj_t this, ...)
{
    static unsigned int linecnt = 0;
    unsigned char data;

    data = *itrread(this);
    printf("%02hhx ", data);

    if (linecnt++ >= 15)
    {
        linecnt = 0;
        putchar('\n');
    }
        
}

static itrobj_t RtIteratorInit(void)
{
    itrobj_t itr;

    itr = itrcnst(0);
    itrinit(itr, 1, region);

    return itr;
}

static void RtFilling(itrobj_t itr)
{
    itrrst(itr);
    itrcallback(itr, filling, ITR_STEP);
    itrrun(itr, REGION_SIZE);
}

static void RtDump(itrobj_t itr)
{
    itrrst(itr);
    itrcallback(itr, dump, ITR_STEP);
    itrrun(itr, REGION_SIZE);
}

static itrid_t ItrList[2];
static void WriteByte(itrobj_t itr, ...)
{
    if (ItrList[0] == itrgetid(itr))
    {
        *itrread(itr) = 0xAA;
        return;
    }

    if (ItrList[1] == itrgetid(itr))
    {
        *itrread(itr) = 0x00;
        return;
    }
}

static void RtPrintId(itrobj_t itr, ...)
{
    va_list args;
    itrobj_t peer;

    va_start(args, itr);
    peer = va_arg(args, itrobj_t);
    
    itridx(itr, IDX_ITR1_ID);
    itridx(peer, IDX_ITR2_ID);

    ItrList[0] = itrgetid(itr);
    ItrList[1] = itrgetid(peer);

    itrcallback(itr, WriteByte, ITR_STEP);
    itrcallback(peer, WriteByte, ITR_STEP);

    itrrun(peer, 5);

    itrulink(itr, ITR_PEER, peer, NULL);

    va_end(args);
}

static void RtWriteString(itrobj_t itr, char* str, int idx)
{
    itridx(itr, idx);
    itrwrite(itr, str, strlen(str) + 1);
}

static void RtStringCmp(itrobj_t itr, char* str, int idx)
{
    itridx(itr, idx);
    printf("%s %s %s\r\n\r\n",
        itrread(itr),
        (itrcmp(itr, str, ITR_DATA, strlen(str) + 1)) ? "!=" : "==",
        str);
}

int main()
{

    itrobj_t itr1, itr2;

    itrdbg((ITR_L_CALLBACK | ITR_L_STEP), (ITR_L_STDOUT));

    itr1 = RtIteratorInit();
    itr2 = RtIteratorInit();
    itrlink(itr1, ITR_PEER_TO_PEER, itr2);
    RtFilling(itr1);
    RtDump(itr1);
    RtStringCmp(itr1, STR_HELLO, IDX_HELLO);
    RtWriteString(itr1, STR_HELLO, IDX_HELLO);
    RtDump(itr1);
    RtStringCmp(itr1, STR_HELLO, IDX_HELLO);
    RtPrintId(itr1, itr2, NULL);
    RtDump(itr1);

    ITRLOG(ITR_L_DUMP, "", itr1);
    ITRLOG(ITR_L_DUMP, "", itr2);

    itrdstr(itr1);
    itrdstr(itr2);
}
#endif