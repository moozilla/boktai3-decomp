#ifndef GUARD_WEATHER_STATE_H
#define GUARD_WEATHER_STATE_H

#include "engine/task.h"
#include "engine/video/sprite_oam.h"

struct SpriteEffectTileResource;

struct WeatherParticle {
    u8 active, timer, state, endpoint;
    struct ContactPosition positions[2];
    struct SpriteDrawable sprite;
};

struct WeatherState {
    struct Task task;
    u32 phaseTimer;
    u8 entered, phase;
    u16 thunderCountdown;
    u8 enabled, flags;
    u16 stormLight, scroll;
    u8 alternateScroll, unknown27;
    u16 stormDuration, stormDurationRange;
    u16 thunderInterval, thunderIntervalRange;
    u16 stormVolumeScale, stormVolume;
    s32 scrollX[2], scrollY[2];
    s32 drift, targetDrift, driftTimer;
    s32 tilemapLayer;
    void *onStormStart, *onStormEnd;
    u32 frame;
    void *tilemap;
    u16 particleDelay, unknown66;
    u8 unknown68[0x100];
    const struct SpriteEffectTileResource *particleResource;
    struct WeatherParticle particles[32];
};

void Weather_EnterPhase(struct WeatherState *, u32);

#endif
