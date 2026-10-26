#include "global.h"
#include "engine.h"

struct Camera0822725C {
    u8 pad0[0x7e];
    u16 amp;
    s16 offX;
    s16 offY;
    u8 active;
    u8 flag85;
    u16 cur;
    u16 target;
    u16 steps;
};
extern const u16 gUnk_0203B400[];
extern s16 gUnk_030042A0[];
s32 Div(s32, s32);
s32 DivArmMod(s32, s32);
void Camera_NotifyMoveComplete(struct Camera0822725C *);

u32 Camera_UpdateShake(struct Camera0822725C *c)
{
    if (c->active) {
        if (c->steps) {
            c->cur = Div(c->cur * c->steps + c->target, c->steps + 1);
            c->amp = c->cur;
            c->steps--;
            if (c->steps == 0) {
                if (c->flag85) {
                    c->active = 0;
                    c->flag85 = 0;
                    c->cur = 0;
                    c->target = 0;
                } else
                    c->cur = c->target;
                Camera_NotifyMoveComplete(c);
            }
        } else
            c->amp = c->cur;
    }
    if (c->amp) {
        c->offX = DivArmMod(c->amp, gUnk_0203B400[gRandomIndex = (gRandomIndex + 1) & 0x3FF]) -
                  (c->amp >> 1);
        c->offY = DivArmMod(c->amp, gUnk_0203B400[gRandomIndex = (gRandomIndex + 1) & 0x3FF]) -
                  (c->amp >> 1);
        gUnk_030042A0[0] += c->offX;
        gUnk_030042A0[2] += c->offY;
        c->amp = 0;
    } else {
        c->offX = 0;
        c->offY = 0;
    }
}
