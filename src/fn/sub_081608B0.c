#include "global.h"
#include "engine.h"
// MATCHING_CONCESSION: register-pinning

struct Vec {
    u32 x : 16;
    u32 y : 16;
    u32 z : 16;
    u32 pad : 16;
};
extern const s16 gUnk_086149C4[];
s32 Mod(s32, s32);
s32 Div(s32, s32);
void sub_081606C0(u8 *, struct Vec *, s32, void *, s32, s32);
#define W(n) (*(u32 *)(p + (n)))
#define I(n) (*(s32 *)(p + (n)))
#define P(n) (*(u8 **)(p + (n)))
#define D(n) (*(u16 *)(P(0x594) + (n)))
static inline s32 shift12(s32 x)
{
    if (x >= 0)
        return x >> 12;
    return -((-x) >> 12);
}
#define SIN(n) gUnk_086149C4[(n) & 255]
static inline s32 shift_distance(s32 x)
{
    if (x >= 0)
        x = x >> 12;
    else
        x = -((-x) >> 12);
    return x;
}
struct Vec16 {
    u16 x;
    u16 y;
    u16 z;
};
union VecU {
    struct Vec v;
    struct Vec16 h;
};
static inline u16 mask8(s32 x)
{
    return x & 255;
}
s32 PlayerAttackEffect_EmitPaletteBaseTrail(u8 *p)
{
    union VecU a, b, c;
    u32 state = W(0x59C);
    if (state == 1) {
        s32 step, angle;
        if (W(0x584) == 0) {
            gRandomIndex = (gRandomIndex + 1) & 1023;
            I(0xC0) = Mod(*(u16 *)((gRandomIndex << 1) + RANDOM_TABLE_ADDR), 10) + 230;
        }
        angle = ((*P(0x58C) + 5) & 7) << 5;
        step = Div(768, D(0x10));
        if (D(0x14) > 1)
            goto done;
        if (D(0xC) > 2)
            goto done;
        {
            s32 a6, m;
            if (*P(0x590)) {
                s32 t = W(0x584) * step;
                s32 h = t + step / 2;
                s32 k = -256;
                a6 = angle - (h + k) + 32;
                m = 255;
                a6 &= m;
                angle = angle - (t + k) + 32;
            } else {
                s32 t = W(0x584) * step;
                s32 h = t + step / 2;
                s32 k = 256;
                a6 = angle + (h + k) - 32;
                m = 255;
                a6 &= m;
                angle = angle + (t + k) - 32;
            }
            angle = angle & m;
            a.v.x = D(0x20) + shift12(I(0xC0) * SIN(angle + 64));
            a.v.y = D(0x22) + 200;
            a.v.z = D(0x24) + shift12(I(0xC0) * gUnk_086149C4[angle]);
            sub_081606C0(p, &a.v, 5, p + 0x18, 0, 0);
            a.h.x = D(0x20) + shift12(I(0xC0) * SIN(a6 + 64));
            a.h.y = D(0x22) + 200;
            a.h.z = D(0x24) + shift12(I(0xC0) * gUnk_086149C4[a6]);
            sub_081606C0(p, &a.v, 5, p + 0x18, 0, 0);
        }
        W(0x584)++;
    } else if (state == 2) {
        if (W(0x584) == 0)
            I(0xC0) = 290;
        if ((u16)(D(0x14) - 2) > 2)
            goto done;
        {
            register s32 angle asm("r9");
            s32 a64;
            s32 pitch, distance;
            angle = ((*P(0x58C) + 5) & 7) * 32;
            pitch = (306 - W(0x584) * 10) & 255;
            b.v.y = D(0x22) + shift12(I(0xC0) * SIN(pitch)) + 190;
            distance = shift_distance(I(0xC0) * SIN(pitch + 64));
            {
                s32 temporary = angle + 64;
                s32 product;
                product = distance * SIN(temporary);
                a64 = temporary;
                b.v.x = D(0x20) + shift12(product) - 16;
            }
            b.v.z = D(0x24) + shift12(distance * gUnk_086149C4[angle]) + 16;
            sub_081606C0(p, &b.v, 5, p + 0x18, 0, 0);
            pitch = (311 - W(0x584) * 10) & 255;
            b.h.y = D(0x22) + shift12(I(0xC0) * SIN(pitch)) + 190;
            distance = shift_distance(I(0xC0) * SIN(pitch + 64));
            b.h.x = D(0x20) + shift12(SIN(a64) * distance) - 16;
            b.h.z = D(0x24) + shift12(distance * gUnk_086149C4[angle]) + 16;
            sub_081606C0(p, &b.v, 5, p + 0x18, 0, 0);
        }
        W(0x584)++;
    } else if (state == 3) {
        s32 angle;
        if (W(0x584) == 0)
            I(0xC0) = 60;
        if (D(0x14) > 2)
            I(0xC0) -= 70;
        else
            I(0xC0) += 35;
        angle = ((*P(0x58C) + 5) & 7) * 32;
        c.v.x = D(0x20) + shift12(I(0xC0) * SIN(angle + 64)) - 16;
        c.v.y = D(0x22) + 200;
        c.v.z = D(0x24) + shift12(I(0xC0) * gUnk_086149C4[angle]) + 16;
        sub_081606C0(p, &c.v, 5, p + 0x18, 0, 0);
        W(0x584)++;
    } else
        W(0x584) = 0;
done:
    return 0;
}
