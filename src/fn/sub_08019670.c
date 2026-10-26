#include "global.h"
#include "ram.h"

struct S {
    u8 f[0x1fc];
    u32 a;
};
void Heap_Free(u32);
u32 TileAnimation_Destroy(struct S *p)
{
    if (p->a != 0) {
        Heap_Free(p->a);
        p->a = 0;
    }
    return gTileAnimation = 0;
}
