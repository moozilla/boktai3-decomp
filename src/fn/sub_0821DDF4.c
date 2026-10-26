#include "global.h"
struct A {
    u8 p[0xc];
    void *fc;
};
extern struct A *gTerrainMap;
extern u32 gUnk_03001684;
void Terrain_SetBoxes(void *v)
{
    gTerrainMap->fc = v;
    gUnk_03001684 = 0;
}
