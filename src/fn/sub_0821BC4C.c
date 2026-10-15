#include "global.h"
struct A {
    u8 p[4];
    void *f4;
};
extern struct A *gUnk_030052F4;
void Terrain_SetNeighborOffsets(void);
void sub_0821BBBC(void);
void Terrain_UpdateSize(void);
void Terrain_SetGrid(void *v)
{
    gUnk_030052F4->f4 = v;
    Terrain_SetNeighborOffsets();
    sub_0821BBBC();
    Terrain_UpdateSize();
}
