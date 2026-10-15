#include "global.h"
struct A {
    u8 p[0xc];
    void *fc;
};
extern struct A *gUnk_030052F4;
extern u32 gUnk_03001684;
void Terrain_SetBoxes(void *v)
{
    gUnk_030052F4->fc = v;
    gUnk_03001684 = 0;
}
