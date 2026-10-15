#include "global.h"
struct A {
    u8 p[0x10];
    void *f10;
};
extern struct A *gUnk_030052F4;
void Terrain_SetPaths(void *v)
{
    gUnk_030052F4->f10 = v;
}
