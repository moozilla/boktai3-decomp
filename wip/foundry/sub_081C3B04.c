#include "global.h"

#define ContactImpactEffect_UpdateShake sub_081C3B04

extern u32 gUnk_03005308;
extern const u16 gUnk_0203B400[];
extern u16 gUnk_03005418[];

s32 sub_08249350(s32, s32);

static inline s32 scale(s32 x)
{
    s32 r;
    x *= 3;
    if (x >= 0)
        r = x >> 5;
    else
        r = -((-x) >> 5);
    return r;
}

struct ContactImpactEffect {
    u8 unknown0[0x50];
    u16 screenX, screenY, screenZ, screenPad;
    u8 unknown58[0x42];
    u16 shakeThreshold;
    u8 unknown9C[0x3C];
    u32 shakeFlag;
    s16 worldX, worldY, worldZ, worldPad;
    u8 unknownE4[0x14];
    s32 shakeTimer;
};

s32 sub_081C3B04(struct ContactImpactEffect *effect)
{
    {
        u16 *q = &effect->screenX;
        s16 *p = &effect->worldX;
        s32 x = p[0], z = p[2], y;
        q[0] = scale(x - z);
        z = scale(x + z);
        y = scale(p[1]);
        q[1] = z - y;
        z += y;
        q[0] = q[0] - gUnk_03005418[0] + 0x78;
        q[1] = q[1] - gUnk_03005418[1] + 0x50;
        z -= gUnk_03005418[2];
        q[2] = z;
    }

    {
        s32 *timer = &effect->shakeTimer;

        if (*timer >= effect->shakeThreshold)
            effect->shakeFlag = 1;

        {
            s32 amp = *timer >> 2;
            const u16 *table = gUnk_0203B400;
            s32 raw, angle;
            u16 *px, *py;

            gUnk_03005308 = (gUnk_03005308 + 1) & 0x3ff;
            raw = table[gUnk_03005308];
            px = &effect->screenX;
            angle = raw >> 4;
            *px += sub_08249350(angle, amp);

            gUnk_03005308 = (gUnk_03005308 + 1) & 0x3ff;
            raw = table[gUnk_03005308];
            py = &effect->screenY;
            angle = raw >> 4;
            *py += sub_08249350(angle, amp);
        }

        return ++*timer;
    }
}
