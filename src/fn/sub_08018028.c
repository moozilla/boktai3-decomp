#include "global.h"
#define Weather_Initialize sub_08018028
#include "background_controller.h"
#include "ram.h"
#include "monster_particle_random.h"
struct WeatherState {
    u8 task[0x20];
    u8 unknown20, flags;
    u8 pad22[4];
    u8 night;
    u8 pad27;
    u16 delay, delayRange;
    u16 duration, durationRange;
    u16 spread, spreadZ;
    u32 f34, f38, f3C, f40;
    s32 f44, f48;
    u32 f4C, f50;
    u32 f54, f58;
    u8 pad5C[4];
    u8 *resource;
};

int Script_GetKeywordValue(int, int);
int Script_SeekToKeyword(int);
int Script_GetValue(void);
s32 Mod(s32, s32);
void Weather_SetRandomDuration(struct WeatherState *);
s32 Weather_GetStormVolume(struct WeatherState *);
void Weather_EnterPhase(struct WeatherState *, int);
void sub_08017684(struct WeatherState *);
static inline u16 RAND(void)
{
    gRandomIndex = (gRandomIndex + 1) & 0x3ff;
    return *(const u16 *)((gRandomIndex << 1) + (u32)gRandomTable);
}
s32 Weather_Initialize(struct WeatherState *p)
{
    u8 *resource;
    void *map;
    s32 phase;
    gWeatherState = p;
    if ((p->night = Script_GetKeywordValue(0x73, 0)) != 0)
        resource = sub_08216E54(0xB7D3);
    else
        resource = sub_08216E54(0xEA77);
    p->resource = resource;
    map = Video_GetBackgroundMap(1);
    p->f34 = 0;
    p->f3C = 0;
    p->f38 = 0;
    p->f40 = 0;
    p->f50 = 0;
    if (resource[0] == 'M' && resource[1] == 'P') {
        s32 offset = 0;
        Bg_SetPriority(1, 1);
        Bg_SetupLayers(3, 1, resource, 0, 0, 1, &offset);
    } else {
        Bg_SetupLayer(1, 1, resource, 0, 0, 0, 1, map);
    }
    if (Script_SeekToKeyword(0x72)) {
        p->spread = Script_GetValue();
        p->spreadZ = Script_GetValue();
    } else {
        p->spread = 0x40;
        p->spreadZ = 0x40;
    }
    if (Script_SeekToKeyword(0x69)) {
        p->delay = Script_GetValue();
        p->delayRange = Script_GetValue();
    } else {
        p->delay = 7200;
        p->delayRange = 3600;
    }
    if (gStormTimer > p->delay) {
        gStormTimer = p->delay;
        if (p->delayRange)
            gStormTimer += Mod(RAND(), p->delayRange);
    }
    if (Script_SeekToKeyword(0x63))
        gGameState->counters628[2] = Script_GetValue();
    if (Script_SeekToKeyword(0x74)) {
        p->duration = Script_GetValue();
        p->durationRange = Script_GetValue();
    } else {
        p->duration = 300;
        p->durationRange = 300;
    }
    Weather_SetRandomDuration(p);
    p->unknown20 = Script_GetKeywordValue(0x61, 1);
    {
        // One offset variable walks unk876 then counters628[2]: reassigned, it holds no
        // constant cse could hand to the later unk876 read, which reloads 0x876 as in the ROM.
        u8 *base = (u8 *)gGameState;
        u32 offset = 0x876;
        s32 storm = *(s16 *)(base + offset);
        offset = 0x630;
        *(s32 *)(base + offset) += storm >> 2;
    }
    if (gGameState->counters628[2] >= gWeatherCooldownBase)
        gGameState->counters628[2] = gWeatherCooldownBase;
    phase = 0;
    if (gGameState->unk876 == 0) {
        if (gStormTimer > 0) {
            if (gStormTimer <= 99)
                gStormTimer = 100;
            phase = 3;
        } else if (gGameState->counters628[2] == 0) {
            s32 volume = Weather_GetStormVolume(p);
            if ((RAND() & 0x3F) < volume)
                phase = 3;
        }
    }
    Weather_EnterPhase(p, Script_GetKeywordValue(0x6D, phase));
    p->flags = 0;
    if (Script_GetKeywordValue(0x4E, 0))
        p->flags |= 1;
    if (Script_GetKeywordValue(0x4C, 0))
        p->flags |= 2;
    if (Script_GetKeywordValue(0x52, 0))
        p->flags |= 4;
    if (Script_GetKeywordValue(0x54, 0))
        p->flags |= 8;
    if (p->night)
        p->flags |= 0xC;
    p->f54 = Script_GetKeywordValue(0x43, 0);
    p->f58 = Script_GetKeywordValue(0x4F, 0);
    {
        const u16 *table = gRandomTable;
        p->f4C = (ParticleRandom_ReadDirectSample(table) & 127) + 128;
        p->f44 = (ParticleRandom_ReadDirectSample(table) & 7) - 2;
        p->f48 = (ParticleRandom_ReadDirectSample(table) & 7) - 2;
    }

    sub_08017684(p);
    return 0;
}
