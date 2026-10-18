#include "global.h"
void *ShadowRegistry_GetTask(void);
u8 *ShadowRegistry_FindEntry(void *, int);
void Shadow_Destroy(void *);
void ShadowRegistry_UnlinkEntry(void *, void *);
void Heap_Free(void *);
int ShadowRegistry_Remove(int id)
{
    void *m = ShadowRegistry_GetTask();
    u8 *p;
    if (!m)
        return -1;
    p = ShadowRegistry_FindEntry(m, id);
    if (!p)
        return -1;
    Shadow_Destroy(p + 12);
    ShadowRegistry_UnlinkEntry(m, p);
    Heap_Free(p);
    return 0;
}
