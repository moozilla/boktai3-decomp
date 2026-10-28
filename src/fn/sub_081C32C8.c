#include "global.h"
#include "render.h"
s32 __divsi3(s32, s32);
void OrbitMotion_BeginArc(void *, void *, void *, u32, u32);
void Callback_InvokeActor(void *);
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

static inline void SetCallback(void (*callback)(void), void (**slot)(void))
{
    *slot = callback;
}

#ifdef NONMATCHING
// fuzzy: 63.1%
// fix.py best (sweep after the inlined-insn fix)
// Full arc-motion reconstruction; pointer lifetimes, parameter spills and projection allocation differ.
void SceneTarget_BeginMove(struct ContactImpactEffect *input, struct Vec *source,
                           struct Vec *destination, s32 initialScale, u32 rampTo, u32 arcParameter,
                           u32 trackMode)
{
    struct Vec *from = source;
    struct Vec *to = destination;
    s32 rampFrom = initialScale;
    struct ContactImpactEffect *effect = input;
    struct Vec *origin;
    struct Vec *screen;
    struct Vec *target;
    if (!effect)
        return;
    if (!to)
        return;
    if (from) {
        if (!effect->coordinateMode) {
            struct Vec *out = (struct Vec *)(&effect->worldX);
            FromScreen(out, from, 0);
            origin = out;
            screen = (struct Vec *)(&effect->screenX);
        } else {
            origin = (struct Vec *)(&effect->worldX);
            *origin = *from;
            screen = (struct Vec *)(&effect->screenX);
        }
    } else if (!effect->coordinateMode) {
        origin = (struct Vec *)(&effect->worldX);
        screen = (struct Vec *)(&effect->screenX);
        FromScreen(origin, screen, 512);
    } else {
        s32 height;
        origin = (struct Vec *)(&effect->worldX);
        screen = (struct Vec *)(&effect->screenX);
        height = to->y;
        FromScreen(origin, screen, height);
    }
    if (!effect->coordinateMode) {
        struct Vec *out = (struct Vec *)(&effect->targetX);
        FromScreen(out, to, 0);
        target = out;
    } else {
        target = (struct Vec *)(&effect->targetX);
        *target = *to;
    }
    if (rampFrom < 0)
        effect->rampFrom = (s8)effect->byte3C;
    else
        effect->rampFrom = rampFrom;
    {
        typeof(effect->rampTo) *q1 = &effect->rampTo;
        u8 z1 = 0;
        *q1 = rampTo;
        ToScreen(screen, origin);
        effect->byte3C = effect->rampFrom;
        effect->byte3D = effect->rampFrom;
        OrbitMotion_BeginArc(((u8 *)effect) + 0xA0, origin, target, arcParameter, trackMode);
        effect->shakeThreshold = trackMode;
        *((u32 *)(((u8 *)effect) + 0x34)) &= ~1;
        SetCallback(ContactImpactEffect_UpdateContact, (void *)(((u8 *)effect) + 0xF4));
        effect->shakeTimer = z1;
        effect->shakeFlag = z1;
        Callback_InvokeActor(effect);
    }
}

#else
INCLUDE_ASM("asm/nonmatching", SceneTarget_BeginMove);
#endif
