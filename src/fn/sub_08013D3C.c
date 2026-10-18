#include "global.h"
struct Vec {
    s16 x, y, z, pad;
};
struct Entry {
    u16 id;
    u8 mode, pad;
    struct Vec position;
    u8 shadow[72];
};
void *ShadowRegistry_GetTask(void);
void *ShadowRegistry_FindEntry(void *, int);
void *Heap_Alloc(int);
void Memory_Clear(void *, int);
int Shadow_Initialize(void *, void *, int);
int ShadowRegistry_LinkEntry(void *, void *);
void Heap_Free(void *);
int ShadowRegistry_Create(int id, int mode, struct Vec *position)
{
    void *m = ShadowRegistry_GetTask();
    struct Entry *p;
    struct Vec *dest;
    if (!m)
        return -1;
    if (ShadowRegistry_FindEntry(m, id))
        return -1;
    p = Heap_Alloc(84);
    if (!p)
        return -1;
    Memory_Clear(p, 84);
    p->id = id;
    dest = &p->position;
    p->position = *position;
    if (Shadow_Initialize(p->shadow, dest, mode == 2) < 0)
        goto fail;
    if (ShadowRegistry_LinkEntry(m, p) < 0)
        goto fail;
    p->mode = mode;
    return 0;
fail:
    Heap_Free(p);
    return -1;
}
