#include "global.h"
#include "render.h"

s32 __divsi3(s32, s32);
void MotionTrack_Update(u8 *);

static inline s32 scale(s32 x)
{
    s32 r;
    s32 y = x * 3;
    if (y >= 0)
        r = y >> 5;
    else
        r = -((-y) >> 5);
    return r;
}

static inline s32 scale8(s32 x)
{
    s32 r;
    if (x >= 0)
        r = x >> 8;
    else
        r = -((-x) >> 8);
    return r;
}

struct ContactImpactEffect {
    u8 unknown0[0x3C];
    u8 byte3C, byte3D;
    u8 unknown3E[0x12];
    u16 screenX, screenY, screenZ, screenPad;
    u8 unknown58[0x3C];
    u16 rampFrom, rampTo;
    u8 unknown98[2];
    u16 shakeThreshold;
    u8 unknown9C[0x3C];
    u32 shakeFlag;
    s16 worldX, worldY, worldZ, worldPad;
    u8 unknownE4[0x14];
    s32 shakeTimer;
};

void ContactImpactEffect_UpdateContact(struct ContactImpactEffect *effect)
{
    {
        u8 *base = (u8 *)effect + 0x94;
        s32 rampTo = effect->rampTo;
        s32 threshold = effect->shakeThreshold;
        s32 rampFrom = *(u16 *)base;
        s32 *timerPtr = (s32 *)(base + 0x64);
        s32 timer = *timerPtr;
        s32 result;

        if (timer <= 0)
            result = rampFrom;
        else if (timer >= threshold)
            result = rampTo;
        else {
            s32 delta = rampTo - rampFrom;
            s32 scaled = __divsi3(timer * (delta << 8), threshold);
            result = rampFrom + scale8(scaled);
        }
        effect->byte3C = result;
        effect->byte3D = result;
    }

    {
        u16 *q = &effect->screenX;
        s16 *p = &effect->worldX;
        s32 y;
        s32 x, zz;
        s32 z;
        x = p[0];
        zz = p[2];
        q[0] = scale(x - zz);
        z = scale(({
            s32 t = x + zz;
            t;
        }));
        y = scale(p[1]);
        q[1] = z - y;
        z += y;
        q[0] = q[0] - gCameraOrigin[0] + 0x78;
        q[1] = q[1] - gCameraOrigin[1] + 0x50;
        z -= gCameraOrigin[2];
        q[2] = z;
    }

    MotionTrack_Update((u8 *)effect + 0xa0);

    {
        s32 *timer = &effect->shakeTimer;
        s32 t = ++*timer;

        if (t > effect->shakeThreshold) {
            effect->shakeFlag = 1;
        }
    }
}
