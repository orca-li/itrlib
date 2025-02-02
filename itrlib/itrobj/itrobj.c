#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#include "../include/itrlog.h"
#include "../include/itrobj.h"

static itrid_t autoid = AUTO_ID_BASE;

static itrobj_t PreInit(itrflags_t flags);
static itrobj_t ItrConstructor(itrflags_t flags);
void itrrst(itrobj_t this);
static void _itrpp_skip_assert(itrobj_t this);
static void _itrrst_skip_assert(itrobj_t this);

static itrobj_t (*SwitchableConstructor)(itrflags_t) = PreInit;

static itrbool_t isNotValidObject(itrobj_t this)
{
    if (!this)
        return ITR_TRUE;
    if (this->autoid >= autoid || this->autoid < AUTO_ID_BASE)
    {
        ITRLOG(ITR_L_ERROR, "Invalid id: %d", this->autoid);
        return ITR_TRUE;
    }

#ifdef ITRLIB_ENABLE_ASSERT
    ITR_ASSERT(!this);
#endif
    return ITR_FALSE;
}

static itrobj_t PreInit(itrflags_t flags)
{
    SwitchableConstructor = ItrConstructor;
    return SwitchableConstructor(flags);
}

static itrobj_t ItrConstructor(itrflags_t flags)
{
    itrobj_t this = (itrobj_t)malloc(sizeof(itrclass_t));

    if (!this)
        return NULL;

    this->autoid = autoid++;
    this->step = NULL;

    this->parent = NULL;
    this->child = NULL;
    this->next = NULL;
    this->prev = NULL;

    this->flags = flags;

    ITRLOG(ITR_L_TRACE, "id[%d] created", this->autoid);

    return this;
}

itrobj_t itrcnst(itrflags_t flags)
{
    return SwitchableConstructor(flags);
}

void itrinit(itrobj_t this, itrint_t stepsz, itrptr_t locate, ...)
{
    if (isNotValidObject(this))
        return;

    va_list args;
    va_start(args, locate);

    this->index = 0;
    this->stepsz = stepsz;
    this->base = locate;
    this->locate = this->base;

    if (ITR_CHECK_MASK(this->flags, ITR_LIMITATION))
    {
        this->limite = va_arg(args, itrint_t);
        ITRLOG(ITR_L_TRACE, "id[%d] flag:LIMITATION limite:%d", this->autoid, this->limite);
    }

    ITRLOG(ITR_L_TRACE, "id[%d] count:%d stepsz:%d locate:%p",
           this->autoid, this->index, this->stepsz, this->locate);

    va_end(args);
}

void itrlink(itrobj_t this, itrflags_t type, ...)
{
    if (isNotValidObject(this))
        return;

    va_list args;
    va_start(args, type);
    itrobj_t peer;

    if (ITR_CHECK_MASK(type, ITR_PEER_TO_PEER))
    {
        peer = va_arg(args, itrobj_t);
        peer->prev = this;
        this->next = peer;
        /* peer->next = ? */
        goto exit;
    }

    if (ITR_CHECK_MASK(type, ITR_PARENT_CHILD))
    {
        peer = va_arg(args, itrobj_t);
        if (isNotValidObject(peer))
            goto exit;

        this->child = peer;
        peer->parent = this;
        ITRLOG(ITR_L_TRACE, "id[%d] [link] parent(%d)->child(%d)", this->autoid, this->autoid, this->child->autoid);
        goto exit;
    }

exit:
    va_end(args);
}

static void ChildUnlinkHandler(itrobj_t this)
{
    if (!this->child)
        return;
#ifdef ITRLIB_DEBUG
    itrobj_t dbg_child = this->child;
#endif

    ITRLOG(ITR_L_TRACE, "id[%d] [unlink:1] this(%d)->child(%p) child(%d)->parent(%p)",
           this->autoid, this->autoid, this->child, dbg_child->autoid, dbg_child->parent);

    this->child->parent = NULL;
    this->child = NULL;

    ITRLOG(ITR_L_TRACE, "id[%d] [unlink:2] this(%d)->child(%p) child(%d)->parent(%p)",
           this->autoid, this->autoid, this->child, dbg_child->autoid, dbg_child->parent);
}

static void ParentUnlinkHandler(itrobj_t this)
{
    if (!this->parent)
        return;
#ifdef ITRLIB_DEBUG
    itrobj_t dbg_parent = this->parent;
#endif
    ITRLOG(ITR_L_TRACE, "id[%d] [unlink:1] this(%d)->parent(%p) parent(%d)->child(%p)",
           this->autoid, this->autoid, this->parent, dbg_parent->autoid, dbg_parent->child);

    this->parent->child = NULL;
    this->parent = NULL;

    ITRLOG(ITR_L_TRACE, "id[%d] [unlink:2] this(%d)->parent(%p) parent(%d)->child(%p)",
           this->autoid, this->autoid, this->parent, dbg_parent->autoid, dbg_parent->child);
}

void itrulink(itrobj_t this, itrflags_t type, ...)
{
    if (isNotValidObject(this))
        return;

    ITRLOG(ITR_L_TRACE, "id[%d] [unlink:0]", this->autoid);

    if (ITR_CHECK_MASK(type, ITR_PARENT))
        ParentUnlinkHandler(this);
    if (ITR_CHECK_MASK(type, ITR_CHILD))
        ChildUnlinkHandler(this);
}

static void OverflowHandler(itrobj_t this)
{
    if (!ITR_CHECK_MASK(this->flags, ITR_LIMITATION) || this->index < this->limite)
        return;

    itrrst(this);
    ITRLOG(ITR_L_TRACE, "id[%d] index(%d) >= limite(%d)",
           this->autoid, this->index, this->limite);

    _itrpp_skip_assert(this->parent);
}

static void ResetChildHandler(itrobj_t this)
{
    _itrrst_skip_assert(this->child);
}

static void StepCallback(itrobj_t this)
{
    if (this->step)
    {
        this->step(this);
        ITRLOG(ITR_L_CALLBACK, "id[%d] [call] %p", this->autoid, this->step);
    }
}

static void __itrpp__(itrobj_t this)
{
    if (isNotValidObject(this))
        return;

    this->index++;
    this->locate += this->stepsz;
    StepCallback(this);
    ITRLOG(ITR_L_STEP, "id[%d] [pp: itr++] count:%d locate:%p", this->autoid, this->index, this->locate);

    OverflowHandler(this);
    ResetChildHandler(this);
}

static void __itrpp(itrobj_t this, ...)
{
    __itrpp__(this);
}

static void _itrpp_skip_assert(itrobj_t this)
{
    if (!this)
        return;

    __itrpp(this);
}

void itrpp(itrobj_t this)
{
    if (isNotValidObject(this))
        return;

    _itrpp_skip_assert(this);
}

static void __itrmm__(itrobj_t this)
{
    if (isNotValidObject(this))
        return;

    this->index--;
    this->locate -= this->stepsz;
    StepCallback(this);
    ITRLOG(ITR_L_STEP, "id[%d] [mm: itr--] count:%d locate:%p", this->autoid, this->index, this->locate);

    OverflowHandler(this);
    ResetChildHandler(this);
}

static void __itrmm(itrobj_t this, ...)
{
    __itrmm__(this);
}

static void _itrmm_skip_assert(itrobj_t this)
{
    if (!this)
        return;

    __itrmm(this);
}

void itrmm(itrobj_t this)
{
    if (isNotValidObject(this))
        return;

    _itrmm_skip_assert(this);
}

static void _itrmov_skip_assert(itrobj_t this, itrint_t addend)
{
    if (!this)
        return;

    this->index += addend;
    this->locate += addend * this->stepsz;

    ITRLOG(ITR_L_TRACE, "id[%d] [mov: itr += %d] index:%d locate:%p",
           this->autoid, addend, this->index, this->locate);
}

void itrmov(itrobj_t this, itrint_t addend)
{
    if (isNotValidObject(this))
        return;

    _itrmov_skip_assert(this, addend);
}

static void _itridx_skip_assert(itrobj_t this, itrint_t index)
{
    if (!this)
        return;

    if (this->index < index)
    {
        this->locate += this->stepsz * (index - this->index);
    }
    else
    {
        this->locate -= this->stepsz * (this->index - index);
    }
    this->index = index;

    ITRLOG(ITR_L_TRACE, "id[%d] [idx: itr[%d]]", this->autoid, index);
}

void itridx(itrobj_t this, itrint_t index)
{
    if (isNotValidObject(this))
        return;

    _itridx_skip_assert(this, index);
}

static void _itrset_skip_assert(itrobj_t this, itrobj_t source)
{
    if (!this || !source)
        return;

    this->index = source->index;
    this->locate = source->locate;
    ITRLOG(ITR_L_TRACE, "id[%d] [set: itr = src(id[%d])]", this->autoid, source->autoid);
}

void itrset(itrobj_t this, itrobj_t source)
{
    if (isNotValidObject(this) || isNotValidObject(source))
        return;

    if (this->stepsz != source->stepsz)
    {
        ITRLOG(ITR_L_ERROR, "id[%d] [set: itr = src(id[%d])] (this->stepsz(%d) != source->stepsz(%d))",
               this->autoid, source->autoid, this->stepsz, source->stepsz);
#ifdef ITR_ASSERT_ENABLE
        ITR_ASSERT(FALSE);
#endif
        return;
    }
    _itrset_skip_assert(this, source);
}

void itrcallback(itrobj_t this, itrcall_t call, itrflags_t type)
{
    if (isNotValidObject(this))
        return;

    if (ITR_CHECK_MASK(type, ITR_STEP))
        this->step = call;
    
    ITRLOG(ITR_L_TRACE, "id[%d] callback:%p", this->autoid, this->step);
}

void itrrun(itrobj_t this, itrint_t limit)
{
    ITRLOG(ITR_L_TRACE, "id[%d] [run] limit:%d", this->autoid, limit);
    while (limit--)
    {
        itrpp(this);
    }
}

static void _itrrst_skip_assert(itrobj_t this)
{
    if (!this)
        return;
#ifdef ITRLIB_DEBUG
    itrptr_t locate;
    locate = this->locate;
#endif    

    this->index = 0;
    this->locate = this->base;

    ITRLOG(ITR_L_TRACE, "id[%d] [rst: itr &= ~itr] from locate(%p) to base(%p)",
           this->autoid, locate, this->base);
}

void itrrst(itrobj_t this)
{
    if (isNotValidObject(this))
        return;

    _itrrst_skip_assert(this);
}

void itrbase(itrobj_t this)
{
    if (isNotValidObject(this))
        return;

    itrptr_t prev = this->base;
    this->base = this->locate;

    ITRLOG(ITR_L_TRACE, "id[%d] [base: &itr[0] = &itr[c]] new base:%p prev base:%p",
           this->autoid, this->base, prev);
}

void itrdstr(itrobj_t this)
{
    if (isNotValidObject(this))
        return;

    itrid_t id = this->autoid;

    free(this);

    ITRLOG(ITR_L_TRACE, "id[%d] destroyed", id);
}

itrid_t itrgetid(itrobj_t this)
{
    return this->autoid;
}

itrptr_t itrread(itrobj_t this)
{
    return this->locate;
}

itrint_t itrindex(itrobj_t this)
{
    return this->index;
}

itrptr_t itrhook(itrobj_t this, itrint_t index)
{
    itrint_t base;
    itrptr_t data;

    base = this->index;
    itridx(this, index);
    data = itrread(this);
    itridx(this, base);

    return data;
}

static itrint_t MemoryCompareHandler(itrobj_t this, itrobj_t source)
{
    return memcmp(this, source, sizeof(itrclass_t));
}

static itrint_t IndexCompareHandler(itrobj_t this, itrobj_t source)
{
    return this->index - source->index;
}

static itrint_t DataCompareHandler(itrobj_t this, void *data, itrint_t datasz)
{
    return memcmp(this->locate, data, datasz);
}

static itrint_t LocateCompareHandler(itrobj_t this, itrobj_t source)
{
    return this->locate - source->locate;
}

itrint_t itrcmp(itrobj_t this, void *source, itrflags_t flags, ...)
{
    if (isNotValidObject(this))
        goto error;
    if (!ITR_CHECK_MASK(flags, ITR_DATA) && (isNotValidObject(source)))
        goto error;

    itrint_t ret;
    va_list args;
    itrint_t datasz;

    if (ITR_CHECK_MASK(flags, ITR_DATA))
    {
        va_start(args, flags);
        datasz = va_arg(args, itrint_t);
        ret = DataCompareHandler(this, source, datasz);
        goto exit;
    }

    if (ITR_CHECK_MASK(flags, ITR_INDEX))
    {
        ret = IndexCompareHandler(this, source);
        goto exit;
    }

    if (ITR_CHECK_MASK(flags, ITR_MEMORY))
    {
        ret = MemoryCompareHandler(this, source);
        goto exit;
    }

    if (ITR_CHECK_MASK(flags, ITR_LOCATE))
    {
        ret = LocateCompareHandler(this, source);
        goto exit;
    }

error:
    ITRLOG(ITR_L_TRACE, "id[%d] [cmp] error", this->autoid);
    return ITR_COMPARE_ERROR;
exit:
    va_end(args);
    return ret;
}

void itrwrite(itrobj_t this, void *data, itrint_t datasz)
{
    if (isNotValidObject(this))
        return;

    memcpy(this->locate, data, datasz);
    ITRLOG(ITR_L_TRACE, "id[%d] [write: *itr = data] %d bytes written to %p",
           this->autoid, datasz, this->locate);
}