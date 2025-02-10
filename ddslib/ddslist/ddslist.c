#include "../include/ddsdef.h"
#include "../include/ddslist.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#define public
#define protected
#define private

typedef struct DDS_LIST_CLASS
{
public
protected
    ddsflags_t type;
    void *data;
    ddsint_t datasz;
private
    struct DDS_LIST_CLASS *next;
    struct DDS_LIST_CLASS *prev;
    struct DDS_LIST_CLASS *head;
} ddsclass_t;
typedef ddsclass_t *ddsobj_t;

static void TearFromRing(ddsobj_t this)
{
    this->head = this;
    this->next = NULL;
    this->prev = NULL;
}

void *ListConstructor(ddsflags_t flags)
{
    ddsobj_t this;

    this = (ddsobj_t)malloc(sizeof(ddsclass_t));
    if (!this)
        return NULL;

    this->type = flags;
    this->head = this;
    TearFromRing(this);

    return this;
}

void ListInsert(void *addr, DDS_INSERT_TYPES flags, void *data, ...)
{
    ddsobj_t this;

    if (!addr)
        return;

    this = addr;
    if (flags == DDS_INS_DATA)
    {
        va_list args;
        ddsint_t datasz;

        va_start(args, data);
        datasz = va_arg(args, ddsint_t);
        this->data = data;
        this->datasz = datasz;
        va_end(args);
        return;
    }

    ddsobj_t peer;
    peer = data;
    if (flags == DDS_INS_REPLACE)
    {
        peer->head = this->head;
        peer->next = this->next;
        peer->prev = this->prev;
        TearFromRing(this);
        return;
    }

    if (flags == DDS_INS_NEXT)
    {
        if (this->next)
        {
            ddsobj_t next;
            next = this->next;
            this->next = peer;
            peer->head = this->head;
            peer->prev = this;
            peer->next = next;
            return;
        }
        else
        {
            peer->head = this->head;
            this->next = peer;
            peer->prev = this;
            return;
        }
    }
}

static ddsobj_t GetHeadDdsObject(ddsobj_t this)
{
    if (this->type == DDS_TYPE_RING)
        return this;

    ddsobj_t head;
    head = this;
    while (head->prev != NULL)
    {
        head = head->prev;
    }

    return head;
}

void *ListGet(void *addr, DDS_GET_REQUESTS request)
{
    if (!addr)
        return NULL;

    ddsobj_t this;
    this = addr;
    if (request == DDS_GET_DATA)
        return this->data;

    if (request == DDS_GET_NEXT_DATA)
        return this->next->data;

    if (request == DDS_GET_NEXT_DDS)
    {
        if (this->type == DDS_TYPE_RING)
        {
            if (this->next != NULL && this->next != this)
                return this->next;
            else
                return this->head;
        }

        if (this->type == DDS_TYPE_LIST)
            return this->next;
    }

    if (request == DDS_GET_PREV_DATA)
        return this->prev->data;

    if (request == DDS_GET_HEAD_DDS)
        return GetHeadDdsObject(this);

    if (request == DDS_GET_HEAD_DATA)
        return (ddsobj_t)GetHeadDdsObject(this)->data;

    return NULL;
}

void ListDetach(void *addr, DDS_DETACH_REQUESTS request)
{
    ddsobj_t this;
    if (!addr)
        return;

    this = addr;
    if (request == DDS_DETACH_THIS)
    {
        ddsobj_t temp = this->next;
        if (this->prev != NULL)
            this->prev->next = this->next;
        if (this->next != NULL)
            this->next->prev = this->prev;
        TearFromRing(this);
        if (!temp)
            return;
        do
        {
            temp->head = this;
            temp = ListGet(temp->next, DDS_GET_NEXT_DDS);
        } while (NULL != temp);
    }
}

void ListDestructor(void *addr)
{
    free(addr);
}