#include <stdlib.h>
#include "../include/ddsdef.h"
#include "../include/ddslist.h"

#define public
#define protected
#define private

#define RingConstructor ListConstructor
#define RingInsert ListInsert
#define RingGet ListGet
#define RingDetach ListDetach
#define RingDestructor ListDestructor

typedef enum DDS_METHOD_LIST
{
    DDS_METHOD_CONSTRUCTOR = 0,
    DDS_METHOD_INSERT = 1,
    DDS_METHOD_GET = 2,
    DDS_METHOD_DETACH = 3,
    DDS_METHOD_DESTRUCTOR, // last
    DDS_METHOD_COUNT
} ddsmethods_t;

typedef struct DDS_CLASS
{
public
protected
    ddsflags_t type;
private
} ddsclass_t;
typedef ddsclass_t *ddsobj_t;

typedef void* (*dds_constructor_t)(ddsflags_t); // 0
typedef void (*dds_insert_t)(void*, DDS_INSERT_TYPES, void*, ...); // 1
typedef void* (*dds_get_t)(void*, DDS_GET_REQUESTS); // 2
typedef void (*dds_destructor_t)(void*);
typedef void (*dds_detach_t)(void*, DDS_DETACH_REQUESTS);

static dds_constructor_t ConstructorList[] = {
    ListConstructor,
    RingConstructor
};
static dds_insert_t InsertList[] = {
    ListInsert,
    RingInsert
};
static dds_get_t GetList[] = {
    ListGet,
    RingGet
};
static dds_destructor_t DestructorList[] = {
    ListDestructor,
    RingDestructor
};
static dds_detach_t DetachList[] = {
    ListDetach,
    RingDetach
};

static void* MethodList[] = {
    ConstructorList, // 0
    InsertList, // 1
    GetList, // 2
    DetachList, // 3
    DestructorList // last
};

static void* GetMethod(ddsmethods_t method, ddsflags_t dds)
{
    if (method < 0 || method >= DDS_METHOD_COUNT)
        return NULL;

    return ((void***)MethodList)[method][dds];
}

ddsobj_t ddscnst(ddsflags_t flags) // 0
{
    dds_constructor_t constructor;

    constructor = GetMethod(DDS_METHOD_CONSTRUCTOR, flags);
    if (constructor == NULL)
        return NULL;

    return constructor(flags);
}

void ddsins(ddsobj_t this, DDS_INSERT_TYPES type, void* data, ddsint_t datasz) // 1
{
    dds_insert_t insert;

    insert = GetMethod(DDS_METHOD_INSERT, this->type);
    if (insert == NULL)
        return;

    insert(this, type, data, datasz);
}

void ddsdetach(ddsobj_t this, DDS_DETACH_REQUESTS request)
{
    dds_detach_t detach;

    detach = GetMethod(DDS_METHOD_DETACH, this->type);
    if (detach == NULL)
        return;

    detach(this, request);
}

void* ddsget(ddsobj_t this, DDS_GET_REQUESTS request)
{
    dds_get_t get;

    get = GetMethod(DDS_METHOD_GET, this->type);
    if (get == NULL)
        return NULL;

    return get(this, request);
}

void ddsdstr(ddsobj_t this) // last
{
    void (*destructor)(ddsobj_t);

    destructor = GetMethod(DDS_METHOD_DESTRUCTOR, this->type);
    if (destructor == NULL)
        return;

    destructor(this);
}