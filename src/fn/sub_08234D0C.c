#include "global.h"
#include "engine_globals.h"
extern u16 gUnk_0203B400[];
s32 Mod(s32, s32);
void Sound_PlaySong(u32);
void HealthRestoreEffect_UpdateRestoration(u8 *);
#define H(o) (*(u16 *)(p + (o)))
static inline s32 rnd(u16 *table)
{
    s32 v;
    gRandomIndex = (gRandomIndex + 1) & 1023;
    v = table[gRandomIndex];
    return v >> 3;
}
void HealthRestoreEffect_UpdateShake(u8 *p)
{
    H(0x270)++;
    if (H(0x270) < 8) {
        s32 range = 8 - H(0x270);
        u16 *table = gUnk_0203B400;
        H(0x3c) = H(0x7c) + Mod(rnd(table), range) - (range >> 1);
        H(0x3e) = H(0x7e) + Mod(rnd(table), range) - (range >> 1);
    }
    if (H(0x270) > 29) {
        *(void (**)(u8 *))(p + 0x274) = HealthRestoreEffect_UpdateRestoration;
        Sound_PlaySong(0x259);
        H(0x270) = 0;
    }
}
