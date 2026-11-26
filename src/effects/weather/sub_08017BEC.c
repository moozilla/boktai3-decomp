#include "global.h"
#include "effects/weather/weather_state.h"
#include "engine/video/render.h"
#include "system/game_state/game_state.h"
#include "gba/io_reg.h"
void Script_Call(void *, int);
int Weather_SpawnTerrainParticles(void *, int, int, int);
static inline void Weather_SetStormActiveFlag(void)
{
    u32 mask = 4096;
    gGameState->flags868 |= mask;
}
int Weather_StormOnset(struct WeatherState *p)
{
    int mask;
    if (p->entered) {
        p->entered = 0;
        Weather_SetStormActiveFlag();
        if (p->onStormStart)
            Script_Call(p->onStormStart, 0);
    }
    mask = 512;
    gDispCntShadow = mask | gDispCntShadow;
    if (p->phaseTimer <= 14) {
        if ((p->phaseTimer & 3) <= 2)
            gDispCntShadow &= ~mask;
    } else if (p->phaseTimer <= 29) {
        if ((p->phaseTimer & 3) <= 1)
            gDispCntShadow &= ~mask;
    } else {
        if (!(p->phaseTimer & 3))
            gDispCntShadow &= ~mask;
    }
    Weather_SpawnTerrainParticles(p, 3, 10, 10);
    if (p->phaseTimer > 44) {
        Weather_EnterPhase(p, 3);
        return 1;
    } else {
        p->tilemapLayer = 0;
        p->phaseTimer++;
        return 0;
    }
}
