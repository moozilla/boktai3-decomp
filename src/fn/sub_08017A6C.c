#include "global.h"
#include "render.h"
#include "ram.h"
#include "engine_globals.h"
#include "monster_particle_random.h"
// confidence=low: checks light, cooldown, and random volume before entering storm phase one.
#define Weather_UpdateStormTrigger sub_08017A6C
struct Weather {
    u8 pad[24];
    u32 timer;
    u8 entered, state;
    u16 countdown;
    u8 enabled, flags;
};
extern u8 *gGameState;
void Palette_SetBrightnessMode(s32);
u32 Weather_TestFlags(u32, u32);
s32 Weather_GetStormVolume(struct Weather *);
void Weather_EnterPhase(struct Weather *, s32);
s32 Mod(s32, s32);
static inline void ClearStormFlag(void)
{
    u32 *flags = (void *)(gGameState + 0x868);
    u32 mask = ~0x1000;
    *flags &= mask;
}
static inline void ClearDisplayFlag(u16 *display, u32 mask)
{
    *display = mask & *display;
}
static inline u32 IsPaused(u32 mask)
{
    return (gEngineFlags | gScenePauseFlags) & mask;
}
static inline s32 *StormTimer(u8 *base)
{
    u32 offset = 0x630;
    s32 *timer;

    timer = (void *)(base + offset);
    return timer;
}
s32 Weather_UpdateStormTrigger(struct Weather *p)
{
    if (p->entered) {
        p->entered = 0;
        ClearStormFlag();
        Palette_SetBrightnessMode(0);
        ClearDisplayFlag(&gDispCntShadow, ~0x200);
    }
    if (!Weather_TestFlags(p->flags, 2)) {
        s32 volume = Weather_GetStormVolume(p);
        s32 light = *(s16 *)(gGameState + 0x876);
        if (!IsPaused(0x400) && p->enabled) {
            if (light > 0) {
                s32 *timer = StormTimer(gGameState);
                *timer += light >> 2;
                {
                    u8 *base = gGameState;
                    timer = StormTimer(base);
                }
                if (*timer >= gWeatherCooldownBase)
                    *timer = gWeatherCooldownBase;
            } else {
                s32 *timer = StormTimer(gGameState);
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
                        s32 *last = StormTimer(gGameState);
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
    p->timer++;
    return 0;
}
