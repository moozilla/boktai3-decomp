#include "global.h"
void *sub_08013C14(void);
u8 *sub_08013C2C(void *, int);
void sub_08013684(void *);
void sub_08013C74(void *, void *);
void Heap_Free(void *);
int ShadowRegistry_Remove(int id)
{
    void *m = sub_08013C14();
    u8 *p;
    if (!m)
        return -1;
    p = sub_08013C2C(m, id);
    if (!p)
        return -1;
    sub_08013684(p + 12);
    sub_08013C74(m, p);
    Heap_Free(p);
    return 0;
}
