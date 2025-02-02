#ifndef ITR_GUID32_5028D25D
#define ITR_GUID32_5028D25D

#include "itrlib/include/itrdef.h"

typedef struct ITERATOR_CLASS itrclass_t;
typedef itrclass_t* itrobj_t;
typedef void (*itrcall_t)(itrobj_t, ...);

itrobj_t itrcnst(itrflags_t);
void itrdstr(itrobj_t);
void itrinit(itrobj_t this, itrint_t stepsz, itrptr_t locate, ...);

void itrpp(itrobj_t);
void itrmm(itrobj_t);
void itradd(itrobj_t, itrint_t addend);
void itrmov(itrobj_t, itrint_t addend);
void itridx(itrobj_t, itrint_t index);
void itrset(itrobj_t, itrobj_t);
void itrrst(itrobj_t);

void itrbase(itrobj_t);

void itrlink(itrobj_t, itrflags_t type, ...);
void itrulink(itrobj_t, itrobj_t);

void itrrun(itrobj_t, itrint_t limit);
void itrcallback(itrobj_t, itrcall_t, itrflags_t type);

itrint_t itrindex(itrobj_t);
itrptr_t itrread(itrobj_t);
itrid_t itrgetid(itrobj_t);
itrptr_t itrhook(itrobj_t, itrint_t index);
itrint_t itrcmp(itrobj_t, void*, itrflags_t, ...);
void itrwrite(itrobj_t, void* data, itrint_t datasz);

#ifdef ITRLIB_DEBUG
#include "itrlib/include/itrlog.h"
#define itrdbg(...) ITR_DBG_CONFIG(__VA_ARGS__)
#else
#define itrdbg(...)
#endif

#endif /* ITR_GUID32_5028D25D */