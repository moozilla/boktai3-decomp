#include "global.h"
struct A {
    u8 p[4];
    void *f4;
};
extern struct A *gUnk_030052F4;
void *ScriptProc_Start(u32, u16);
void Terrain_SetNeighborOffsets(void);
void sub_0821BBBC(void);
void Terrain_UpdateSize(void);
u32 Terrain_LoadGridResource(u16 v)
{
    gUnk_030052F4->f4 = ScriptProc_Start(0xAE1B, v);
    Terrain_SetNeighborOffsets();
    sub_0821BBBC();
    Terrain_UpdateSize();
    return 0;
}
