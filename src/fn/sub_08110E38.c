#include "global.h"
void Heap_Free(u32);
extern u32 gSceneCamera;
struct S {
    u8 f[0x24];
    u32 a;
    u8 g[8];
    u32 b;
};
u32 SceneTilemap_Destroy(struct S *p)
{
    Heap_Free(p->a);
    Heap_Free(p->b);
    return gSceneCamera = 0;
}
