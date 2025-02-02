#include <itrlib.h>
#include <stdio.h>
#include <string.h>

#define STR_HELLO "Hello, world!"
#define IDX_HELLO 0x22

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
    itrobj_t iterator;

    itrdbg(ITR_L_CALLBACK | ITR_L_STEP, ITR_L_STDOUT);

    iterator = RtIteratorInit();
    RtFilling(iterator);
    RtDump(iterator);
    RtStringCmp(iterator, STR_HELLO, IDX_HELLO);
    RtWriteString(iterator, STR_HELLO, IDX_HELLO);
    RtDump(iterator);
    RtStringCmp(iterator, STR_HELLO, IDX_HELLO);

    ITRLOG(ITR_L_DUMP, "", iterator);

    itrdstr(iterator);
}