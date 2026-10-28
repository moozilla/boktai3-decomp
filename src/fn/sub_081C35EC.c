#include "global.h"
#include "render.h"
s32 __divsi3(s32, s32);
void OrbitMotion_BeginDistanceWave(void *, void *, void *, u32, u32);
void ContactImpactEffect_UpdateContact(void);
struct ContactImpactEffect {
    u8 unknown0[0x3C];
    u8 byte3C, byte3D;
    u8 unknown3E[0x12];
    u16 screenX, screenY, screenZ, screenPad;
    u8 unknown58[0x3C];
    u16 rampFrom, rampTo;
    u16 coordinateMode;
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
static inline s32 Half(s32 value)
{
    return value >= 0 ? value >> 1 : -(-value >> 1);
}
static inline s32 Scale(s32 value)
{
    s32 scaled = value * 3;
    return scaled >= 0 ? scaled >> 5 : -(-scaled >> 5);
}
static inline void FromScreen(struct Vec *out, const struct Vec *screen, s32 offset)
{
    s32 x = screen->x + (s16)gCameraOrigin[0] - 120;
    s32 y = screen->y + (s16)gCameraOrigin[1] - 80;
    out->x = Half(__divsi3((x + y) << 5, 3) + offset);
    out->y = offset;
    out->z = Half(__divsi3((y - x) << 5, 3) + offset);
}
static inline void ToScreen(struct Vec *screen, struct Vec *world)
{
    s32 x = world->x;
    s32 z = world->z;
    s32 sum;
    s32 y;
    screen->x = Scale(x - z);
    sum = Scale(x + z);
    y = Scale(world->y);
    screen->y = sum - y;
    sum += y;
    screen->z = sum;
    screen->x = screen->x - gCameraOrigin[0] + 120;
    screen->y = screen->y - gCameraOrigin[1] + 80;
    screen->z = screen->z - gCameraOrigin[2];
}

void sub_081C3B04(void);
static inline void SetCallback(void (*callback)(void), void (**slot)(void))
{
    *slot = callback;
}

// confidence=low: begins a stationary world-position effect with a timed callback.
#define ContactImpactEffect_BeginTimedPosition sub_081C35EC
void ContactImpactEffect_BeginTimedPosition(struct ContactImpactEffect *input, struct Vec *source,
                                            u32 time)
{
    struct Vec *from = source;
    u32 duration = time;
    struct ContactImpactEffect *effect = input;
    if (effect) {
        if (from) {
            if (!effect->coordinateMode)
                FromScreen((struct Vec *)&effect->worldX, from, 0);
            else
                *(struct Vec *)&effect->worldX = *from;
        } else
            FromScreen((struct Vec *)&effect->worldX, (struct Vec *)&effect->screenX, 0);
        effect->shakeThreshold = duration;
        *(u32 *)((u8 *)effect + 0x34) &= ~1;
        SetCallback(sub_081C3B04, (void *)((u8 *)effect + 0xF4));
        effect->shakeTimer = 0;
        effect->shakeFlag = 0;
    }
}
