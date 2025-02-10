#ifndef DDS_GUID32_3504A0DB
#define DDS_GUID32_3504A0DB

#include "ddsdef.h"

void* ListConstructor(ddsflags_t);
void ListInsert(void*, DDS_INSERT_TYPES flags, void* data, ...);
void* ListGet(void*, DDS_GET_REQUESTS);
void ListDetach(void*, DDS_DETACH_REQUESTS);
void ListDestructor(void*);

#endif /* DDS_GUID32_3504A0DB */