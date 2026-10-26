#include "global.h"
#include "ram.h"
struct N {
    u8 f[0x18];
    struct N *next;
    u8 g[0x34];
    struct N *n50;
};
void Shadow_Destroy(u8 *);
void ShadowRegistry_UnlinkEntry(void *, void *);
void Heap_Free(void *);
u32 ShadowRegistry_OnDestroy(u8 *p)
{
    struct N *e = *(struct N **)(p + 0x18);
    struct N *nx;
    while (e != 0) {
        nx = e->n50;
        Shadow_Destroy((u8 *)e + 0xc);
        ShadowRegistry_UnlinkEntry(p, e);
        Heap_Free(e);
        e = nx;
    }
    {
        u32 z = 0;
        *(u32 *)(p + 0x18) = z;
        gShadowRegistry = z;
    }
    return 0;
}
