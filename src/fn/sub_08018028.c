#include "global.h"
#ifdef NONMATCHING
// fuzzy: 90.9%
// Open: the 0x876 offset constant is kept in r7 across the clamp where the ROM
// rematerializes it, and the final random block reuses the earlier seed address and
// allocates its registers differently (ROM r4 table, r5 seed, r1 index, r3 mask, r2 sample mask).
#include "background_controller.h"
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
extern struct WeatherState *gUnk_02000090;
extern s32 gUnk_0200008C;
extern s32 gUnk_02000080;

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
    gUnk_03005308 = (gUnk_03005308 + 1) & 0x3ff;
    return *(const u16 *)((gUnk_03005308 << 1) + (u32)gUnk_0203B400);
}
s32 sub_08018028(struct WeatherState *p)
{
    u8 *resource;
    void *map;
    s32 phase;
    gUnk_02000090 = p;
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
    if (gUnk_0200008C > p->delay) {
        gUnk_0200008C = p->delay;
        if (p->delayRange)
            gUnk_0200008C += Mod(RAND(), p->delayRange);
    }
    if (Script_SeekToKeyword(0x63))
        *(s32 *)((u8 *)gUnk_02000710 + 0x630) = Script_GetValue();
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
        s32 storm = gUnk_02000710->unk876;
        *(s32 *)((u8 *)gUnk_02000710 + 0x630) += storm >> 2;
    }
    if (*(s32 *)((u8 *)gUnk_02000710 + 0x630) >= gUnk_02000080)
        *(s32 *)((u8 *)gUnk_02000710 + 0x630) = gUnk_02000080;
    phase = 0;
    if (gUnk_02000710->unk876 == 0) {
        if (gUnk_0200008C > 0) {
            if (gUnk_0200008C <= 99)
                gUnk_0200008C = 100;
            phase = 3;
        } else if (*(s32 *)((u8 *)gUnk_02000710 + 0x630) == 0) {
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
        const u16 *table = gUnk_0203B400;
        u32 *seed = &gUnk_03005308;
        u32 index = (*seed + 1) & 0x3FF;
        u32 m;
        p->f4C = (table[index] & 0x7F) + 0x80;
        index = (index + 1) & 0x3FF;
        m = 7;
        p->f44 = (table[index] & m) - 2;
        index = (index + 1) & 0x3FF;
        *seed = index;
        p->f48 = (table[index] & m) - 2;
    }
    sub_08017684(p);
    return 0;
}

#else
INCLUDE_ASM("asm/nonmatching", sub_08018028);
#endif
