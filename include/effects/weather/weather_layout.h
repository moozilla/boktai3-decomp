#ifndef GUARD_INCLUDE_EFFECTS_WEATHER_WEATHER_LAYOUT_H
#define GUARD_INCLUDE_EFFECTS_WEATHER_WEATHER_LAYOUT_H

#include "global.h"

struct Weather {
    u8 pad0[0x1d];
    u8 state;
    u8 pad1E[0x6];
    u16 scroll;
    u8 alternate;
    u8 pad27[0xd];
    s32 x[2];
    s32 y[2];
    s32 drift;
    s32 target;
    s32 timer;
    s32 layer;
    u8 pad54[0x8];
    u32 frame;
};

typedef char WeatherSizeCheck[sizeof(struct Weather) == 0x60 ? 1 : -1];

#endif
