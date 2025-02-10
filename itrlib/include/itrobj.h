#ifndef ITR_GUID32_40D4ED97
#define ITR_GUID32_40D4ED97

#include <ddslib.h>
#include "itrdef.h"

#define public
#define private

#define AUTO_ID_BASE 10000

typedef struct ITERATOR_CLASS
{
private
    itrid_t autoid;
    itrflags_t flags;

    itrptr_t locate;
    itrptr_t base;

    itrint_t limite;
    itrint_t index;
    itrint_t stepsz;

    void (*step)(struct ITERATOR_CLASS *, ...);

    ddsobj_t kin;
    ddsobj_t peer;
} itrclass_t;
typedef itrclass_t *itrobj_t;
typedef void (*itrcall_t)(struct ITERATOR_CLASS *, ...);

#endif /* ITR_GUID32_40D4ED97 */