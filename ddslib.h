#ifndef DDS_GUID32_286F2655
#define DDS_GUID32_286F2655

#include "ddslib/include/ddsdef.h"

typedef struct DDS_CLASS ddsclass_t;
typedef ddsclass_t* ddsobj_t;

ddsobj_t ddscnst(ddsflags_t);
void ddsdstr(ddsobj_t);

ddsbool_t ddsempty(ddsobj_t);
ddsint_t ddssz(ddsobj_t);

void ddsins(ddsobj_t, ddsflags_t, void* data, ...);
void ddsdetach(ddsobj_t, DDS_DETACH_REQUESTS);
void ddsrm(ddsobj_t, ddsflags_t, ...);
void* ddsget(ddsobj_t, DDS_GET_REQUESTS);
void ddsshift(ddsobj_t*);

#endif /* DDS_GUID32_286F2655 */