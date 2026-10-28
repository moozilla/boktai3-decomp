#include "global.h"
#include "engine_globals.h"

// confidence=low: the health burst's shake, as in HealthRestoreEffect_UpdateShake: jitters for
// 8 frames, starts the burst at frame 30 (sound 0x285), fades the screen at frame 29 in mode 1,
// then after frame 44 moves on to the bursts (mode 0) or sub_082356E4.
extern u16 gUnk_0203B400[];
s32 Mod(s32, s32);
void Sound_PlaySong(u32);
void FlickerParticle_StartSecondaryBurst(u8 *);
void HealthBurstEffect_UpdateBursts(u8 *);
void sub_082356E4(u8 *);
void ScreenTransition_SendCommand(s32, s32, s32, s32, s32, s32, s32);
#define H(o) (*(u16 *)(p + (o)))
static inline s32 rnd(u16 *table)
{
    s32 v;
    gRandomIndex = (gRandomIndex + 1) & 1023;
    v = table[gRandomIndex];
    return v >> 3;
}
void sub_08235794(u8 *p)
{
    H(0x34e)++;
    if (H(0x34e) < 8) {
        s32 range = 8 - H(0x34e);
        u16 *table = gUnk_0203B400;
        H(0x3c) = H(0x7c) + Mod(rnd(table), range) - (range >> 1);
        H(0x3e) = H(0x7e) + Mod(rnd(table), range) - (range >> 1);
    }
    if (H(0x34e) == 30) {
        FlickerParticle_StartSecondaryBurst(p);
        Sound_PlaySong(0x285);
        *(u32 *)(p + 0x24) |= 1;
    }
    if (H(0x34c) == 1 && H(0x34e) == 29)
        ScreenTransition_SendCommand(3, 5, 4, 4, 4, 0x1fff, 1);
    if (H(0x34e) > 44) {
        if (H(0x34c) == 0)
            *(void (**)(u8 *))(p + 0x350) = HealthBurstEffect_UpdateBursts;
        else
            *(void (**)(u8 *))(p + 0x350) = sub_082356E4;
        H(0x34e) = 0;
    }
}
