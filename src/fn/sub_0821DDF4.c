#include "global.h"
#include "runtime.h"
struct A {
    u8 p[0xc];
    void *fc;
};
extern struct A *gTerrainMap;
void Terrain_SetBoxes(void *v)
{
    gTerrainMap->fc = v;
    gTrapDispatchStarted = 0;
}
