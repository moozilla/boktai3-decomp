#include "global.h"
#include "runtime.h"
#include "engine_globals.h"
struct A {
    u8 p[0xc];
    void *fc;
};
void Terrain_SetBoxes(void *v)
{
    ((struct A *)gTerrainMap)->fc = v;
    gTrapDispatchStarted = 0;
}
