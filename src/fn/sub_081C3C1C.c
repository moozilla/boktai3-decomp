#include "global.h"
#include "render.h"

s32 __divsi3(s32, s32);
void MotionTrack_Update(u8 *);

static inline s32 scale(s32 value)
{
    s32 r;
    s32 input = value;
    s32 x;
    x = input * 3;
    if (x >= 0)
        r = x >> 5;
    else
        r = -((-x) >> 5);
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
    s16 targetX, targetY, targetZ, targetPad;
    u8 unknownEC[0xC];
    s32 shakeTimer;
};

struct Vec {
    s16 x, y, z, pad;
};
struct PackedVec {
    u32 x : 16;
    u32 y : 16;
    u32 z : 16;
    u32 pad : 16;
};
static inline s32 interpolate(s32 start, s32 end, s32 duration, s32 time)
{
    if (time <= 0)
        return start;
    if (time >= duration)
        return end;
    return start + scale8(((end - start) << 8) * time / duration);
}
void ContactImpactEffect_UpdateInterpolatedPosition(struct ContactImpactEffect *effect)
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
        struct PackedVec pos;
        u16 *q;
        struct Vec *p;
        s32 x;
        s32 z;
        s32 y;
        s32 sum;
        pos.x = interpolate(effect->worldX, effect->targetX, effect->shakeThreshold,
                            effect->shakeTimer);
        pos.y = interpolate(effect->worldY, effect->targetY, effect->shakeThreshold,
                            effect->shakeTimer);
        pos.z = interpolate(effect->worldZ, effect->targetZ, effect->shakeThreshold,
                            effect->shakeTimer);
        q = &effect->screenX;
        p = (struct Vec *)&pos;
        x = ((s16 *)&pos)[0];
        z = p->z;
        q[0] = scale(x - z);
        sum = scale(x + z);
        {
            struct Vec *source = p;
            y = scale(source->y);
        }
        {
            s32 sy = sum - y;
            s32 depth = sum + y;
            q[0] = q[0] - gCameraOrigin[0] + 120;
            q[1] = sy - gCameraOrigin[1] + 80;
            q[2] = depth - gCameraOrigin[2];
        }
    }
    {
        s32 *timer = &effect->shakeTimer;
        s32 t = ++*timer;
        {
            u16 *limitp = &effect->shakeThreshold;
            s32 limit;
            limit = *limitp;
            if (t > limit)
                effect->shakeFlag = 1;
        }
    }
}
