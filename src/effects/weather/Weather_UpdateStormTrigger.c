#include "global.h"
#include "effects/weather/weather_state.h"
#include "engine/video/render.h"
#include "system/game_state/game_state.h"
#include "engine/ram.h"
#include "engine/engine_globals.h"
#include "engine/math/monster_particle_random.h"
#include "engine/engine_pause.h"
// confidence=low: checks light, cooldown, and random volume before entering storm phase one.
void Palette_SetBrightnessMode(s32);
u32 Weather_TestFlags(u32, u32);
s32 Weather_GetStormVolume(struct WeatherState *);
s32 Mod(s32, s32);
static inline void Weather_ClearStormFlag(void)
{
    u32 *flags = &gGameState->flags868;
    u32 mask = ~0x1000;
    *flags &= mask;
}
static inline void Weather_ClearDisplayFlag(u16 *display, u32 mask)
{
    *display = mask & *display;
}
static inline s32 *Weather_CooldownTimer(struct GameState *state)
{
    return &state->counters628[2];
}
s32 Weather_UpdateStormTrigger(struct WeatherState *p)
{
    if (p->entered) {
        p->entered = 0;
        Weather_ClearStormFlag();
        Palette_SetBrightnessMode(0);
        Weather_ClearDisplayFlag(&gDispCntShadow, ~0x200);
    }
    if (!Weather_TestFlags(p->flags, 2)) {
        s32 volume = Weather_GetStormVolume(p);
        s32 light = gGameState->unk876;
        if (!Engine_PauseBits(0x400) && p->enabled) {
            if (light > 0) {
                s32 *timer = Weather_CooldownTimer(gGameState);
                *timer += light >> 2;
                {
                    struct GameState *base = gGameState;
                    timer = Weather_CooldownTimer(base);
                }
                if (*timer >= gWeatherCooldownBase)
                    *timer = gWeatherCooldownBase;
            } else {
                s32 *timer = Weather_CooldownTimer(gGameState);
                if (!*timer) {
                    const u16 *table = gRandomTable;
                    {
                        s32 sample = ParticleRandom_ReadSample(table);
                        sample &= 63;
                        if (sample < volume)
                            goto beginStorm;
                    }
                    *timer = gWeatherCooldownBase;
                    if (gWeatherCooldownRange > 0) {
                        s32 random = Mod(ParticleRandom_ReadSample(table), gWeatherCooldownRange);
                        s32 *last = Weather_CooldownTimer(gGameState);
                        *last += random;
                    }
                } else
                    (*timer)--;
            }
        }
    }
    goto done;
beginStorm:
    Weather_EnterPhase(p, 1);
    return 1;
done:
    p->phaseTimer++;
    return 0;
}
