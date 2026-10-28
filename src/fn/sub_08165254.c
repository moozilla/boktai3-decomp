#include "global.h"
#include "engine_globals.h"
extern u32 gUnk_02000560;
extern u8 *gGameState;
s32 Div(s32, s32);
void Anim_LoadKeyframe(void *, void *, u16, s32);
void SceneHud_PlayActionChangeCue(void *, s32, s32, s32);
struct AnimationFlags {
    u8 pad[8];
    u32 flags;
};
static inline u32 TestFlags(u32 mask, u32 *p)
{
    return mask & *p;
}
struct DigitSprite {
    u32 flags;
    u8 rest[0x5c];
};
static inline void SetDigitVisibility(u8 *p, u32 mask, s32 hide)
{
    struct DigitSprite *s = (struct DigitSprite *)(p + 0xb10);
    s32 i = 2;
    if (hide) {
        do {
            s->flags |= mask;
            s++;
        } while (--i >= 0);
    } else {
        do {
            s->flags &= mask;
            s++;
        } while (--i >= 0);
    }
}
void Hud_UpdateCountdownDigits(u8 *p)
{
    s32 value, hundreds, tens, ones, base;
    u8 *res;
    if (p[0xaff]) {
        if (!*(u32 *)(p + 0xa40) || TestFlags(4, &gUnk_02000560)) {
            SetDigitVisibility(p, 1, 1);
            p[0xaff] = 0;
        } else {
            value = **(u16 **)(p + 0xb04);
            if (value > 59940) {
                SetDigitVisibility(p, 1, 1);
                p[0xb00] = 0;
            } else {
                res = p + 0x64;
                SetDigitVisibility(p, ~1, 0);
                if (value > 0) {
                    value = Div(value - 1, 60) + 1;
                    hundreds = Div(value, 100);
                    base = hundreds * 100;
                    tens = Div(value - base, 10);
                    ones = value - tens * 10 - base;
                } else {
                    hundreds = 0;
                    tens = 0;
                    ones = 0;
                }
                if ((gEngineFlags & 0x1000) || (*(u32 *)(gGameState + 0x868) & 0x400)) {
                    ((struct AnimationFlags *)(p + 0xbc8))->flags |= 1;
                    if (hundreds == 0 && tens == 0) {
                        Anim_LoadKeyframe(p + 0xb08, res, 0x33, 1);
                        Anim_LoadKeyframe(p + 0xb68, res, ones + 0x33, 1);
                    } else {
                        Anim_LoadKeyframe(p + 0xb08, res, tens + 0x29, 1);
                        Anim_LoadKeyframe(p + 0xb68, res, ones + 0x29, 1);
                    }
                } else {
                    if (hundreds == 0 && tens == 0) {
                        Anim_LoadKeyframe(p + 0xb08, res, 0x33, 1);
                        Anim_LoadKeyframe(p + 0xb68, res, 0x33, 1);
                        Anim_LoadKeyframe(p + 0xbc8, res, ones + 0x33, 1);
                    } else {
                        Anim_LoadKeyframe(p + 0xb08, res, hundreds + 0x29, 1);
                        Anim_LoadKeyframe(p + 0xb68, res, tens + 0x29, 1);
                        Anim_LoadKeyframe(p + 0xbc8, res, ones + 0x29, 1);
                    }
                }
                SceneHud_PlayActionChangeCue(p, hundreds, tens, ones);
            }
        }
    }
    if (p[0xb01])
        p[0xb01]--;
}
