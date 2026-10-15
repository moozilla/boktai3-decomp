#include "global.h"
struct A {
    u8 p[0x14];
    void *f14;
};
extern struct A *gUnk_030052F4;
void Terrain_SetNavMeshes(void *v)
{
    gUnk_030052F4->f14 = v;
}
