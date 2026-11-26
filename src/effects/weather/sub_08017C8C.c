#include "global.h"
#include "effects/weather/weather_state.h"
#include "engine/video/render.h"
#include "system/game_state/game_state.h"
#include "system/clock/sun_clock.h"
#include "gba/io_reg.h"
#include "engine/ram.h"
#include "engine/engine_globals.h"
void Palette_SetBrightnessMode(int);
int Weather_TestFlags(int, int);
void Weather_SetRandomDuration(void *);
int Weather_SpawnTerrainParticles(void *, int, int, int);
int Mod(int, int);
void Sound_PlaySong(int);
void ScreenTransition_SendCommand(int, int, int, int, int, int, int);
static inline int Weather_DrawRandomSample(void)
{
    u32 offset;
    gRandomIndex = (gRandomIndex + 1) & 1023;
    offset = gRandomIndex * 2;
    return *(u16 *)((u8 *)gRandomTable + offset);
}
static inline void Weather_EnableActiveStormDisplay(void)
{
    Bg_EnableDisplay(DISPCNT_BG1_ON);
    {
        int bit = 4096;
        gGameState->flags868 |= bit;
    }
}
static inline int Weather_GetAccumulatedLight(struct WeatherState *p)
{
    int v = gGameState->unk876;
    v += p->stormLight;
    return v;
}
static inline int Weather_IsInterrupted(void)
{
    int mask = 1024;
    return (gEngineFlags | gScenePauseFlags) & mask;
}
int Weather_ActiveStorm(struct WeatherState *p)
{
    if (p->entered) {
        p->entered = 0;
        if (!gStormTimer) {
            gStormTimer = p->stormDuration;
            if (p->stormDurationRange)
                gStormTimer += Mod(Weather_DrawRandomSample(), p->stormDurationRange);
        }
        p->stormLight = 0;
        Weather_EnableActiveStormDisplay();
        Palette_SetBrightnessMode(2);
    }
    if (!Weather_TestFlags(p->flags, 8)) {
        if (!p->thunderCountdown) {
            if (!(gEngineFlags & 1024) && gWeatherCollisionTimer > 0) {
                gGrayscaleControl = 5;
                Sound_PlaySong(103);
            }
            Weather_SetRandomDuration(p);
        } else {
            if (gSunClock.phase > 2) {
                if (p->thunderCountdown == 5)
                    ScreenTransition_SendCommand(0, 2, 31, 31, 31, 8191, 0);
                else if (p->thunderCountdown == 3)
                    gWeatherCollisionTimer = 6;
            }
            p->thunderCountdown--;
        }
    }
    if (!Weather_TestFlags(p->flags, 1) && p->phaseTimer > 29) {
        p->stormLight = Weather_GetAccumulatedLight(p);
        if (p->stormLight > 59 || Weather_IsInterrupted()) {
            gStormTimer = 0;
            Weather_EnterPhase(p, 4);
            return 1;
        }
    }
    if (!Weather_TestFlags(p->flags, 2)) {
        if (gStormTimer <= 0) {
            Weather_EnterPhase(p, 4);
            gStormTimer = 0;
            return 1;
        } else
            gStormTimer--;
    }
    Weather_SpawnTerrainParticles(p, (Weather_DrawRandomSample() & 3) + 4, 1, 0);
    p->tilemapLayer = 1 - p->tilemapLayer;
    p->phaseTimer++;
    return 0;
}
