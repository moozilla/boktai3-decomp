#include "global.h"
#include "render.h"
#include "contact_position.h"
s32 __divsi3(s32, s32);
void OrbitMotion_BeginArc(void *, void *, void *, u32, u32);
void Callback_InvokeActor(void *);
void ContactImpactEffect_UpdateContact(void);
struct ContactImpactEffect {
    u8 unknown0[0x34];
    u32 flags;
    u8 unknown38[4];
    u8 byte3C, byte3D;
    u8 unknown3E[0x12];
    struct ContactPosition screen;
    u8 unknown58[0x3C];
    u16 rampFrom, rampTo;
    u16 coordinateMode;
    u16 shakeThreshold;
    u8 unknown9C[0x3C];
    u32 shakeFlag;
    struct ContactPosition world;
    struct ContactPosition target;
    u8 unknownEC[8];
    void (*update)(void);
    s32 shakeTimer;
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
static inline void FromScreen(struct ContactPosition *out, const struct ContactPosition *screen,
                              s32 offset, struct ContactPosition **capture)
{
    s32 x = screen->x + (s16)gCameraOrigin[0] - 120;
    s32 y = screen->y + (s16)gCameraOrigin[1] - 80;
    s32 first = __divsi3((x + y) << 5, 3) + offset;
    *capture = out;
    out->x = Half(first);
    out->y = offset;
    out->z = Half(__divsi3((y - x) << 5, 3) + offset);
}
static inline u32 ToScreen(struct ContactPosition *screen, struct ContactPosition *world)
{
    s32 x = world->x;
    s32 z = world->z;
    s32 sum;
    s32 y;
    u32 zero;
    s32 sy;
    screen->x = Scale(x - z);
    sum = Scale(x + z);
    y = Scale(world->y);
    sy = sum - y;
    zero = 0;
    screen->y = sy;
    sum += y;
    screen->z = sum;
    screen->x = screen->x - gCameraOrigin[0] + 120;
    screen->y = screen->y - gCameraOrigin[1] + 80;
    sum -= gCameraOrigin[2];
    screen->z = sum;
    return zero;
}

static inline void SetCallback(void (*callback)(void), void (**slot)(void))
{
    *slot = callback;
}

#ifdef NONMATCHING
// fuzzy: 96.8%
// Projection-local world pointer and zero choose r4 instead of r5.
void SceneTarget_BeginMove(struct ContactImpactEffect *input, struct ContactPosition *source,
                           struct ContactPosition *destination, s32 initialScale, u32 rampTo,
                           u32 arcParameter, u32 trackMode)
{
    struct ContactPosition *from = source;
    struct ContactPosition *to = destination;
    s32 rampFrom = initialScale;
    struct ContactImpactEffect *effect = input;
    struct ContactPosition *origin;
    struct ContactPosition *target;
    if (!effect)
        return;
    if (!to)
        return;
    if (from) {
        if (!effect->coordinateMode) {
            struct ContactPosition *out = &effect->world;
            FromScreen(out, from, 0, &origin);
        } else {
            struct ContactPosition *out = &effect->world;
            *out = *from;
            origin = out;
        }
    } else if (!effect->coordinateMode) {
        struct ContactPosition *out = &effect->world;
        struct ContactPosition *in = &effect->screen;
        FromScreen(out, in, 0, &origin);
    } else {
        struct ContactPosition *out = &effect->world;
        struct ContactPosition *in = &effect->screen;
        s32 height = to->y;
        FromScreen(out, in, height, &origin);
    }
    if (!effect->coordinateMode) {
        struct ContactPosition *out = &effect->target;
        FromScreen(out, to, 0, &target);
    } else {
        struct ContactPosition *out = &effect->target;
        *out = *to;
        target = out;
    }
    if (rampFrom < 0)
        effect->rampFrom = (s8)effect->byte3C;
    else
        effect->rampFrom = rampFrom;
    {
        typeof(effect->rampTo) *q1 = &effect->rampTo;
        u32 z1;
        *q1 = rampTo;
        z1 = ToScreen(&effect->screen, origin);
        effect->byte3C = effect->rampFrom;
        effect->byte3D = effect->rampFrom;
        OrbitMotion_BeginArc(((u8 *)effect) + 0xA0, origin, target, arcParameter, trackMode);
        effect->shakeThreshold = trackMode;
        effect->flags &= ~1;
        SetCallback(ContactImpactEffect_UpdateContact, &effect->update);
        effect->shakeTimer = z1;
        effect->shakeFlag = z1;
        Callback_InvokeActor(effect);
    }
}
#else
INCLUDE_ASM("asm/nonmatching", SceneTarget_BeginMove);
#endif
