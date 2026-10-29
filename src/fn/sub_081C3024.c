#include "global.h"
#include "render.h"
s32 __divsi3(s32, s32);
void OrbitMotion_BeginDistanceWave(void *, void *, void *, u32, u32);
void ContactImpactEffect_UpdateContact(void);
struct Vec {
    s16 x, y, z, pad;
};

struct ContactImpactEffect {
    u8 unknown0[0x34];
    u32 flags;
    u8 unknown38[4];
    u8 byte3C, byte3D;
    u8 unknown3E[0x12];
    struct Vec screen;
    u8 unknown58[0x3C];
    u16 rampFrom, rampTo;
    u16 coordinateMode;
    u16 shakeThreshold;
    u8 unknown9C[4];
    u8 motion[0x38];
    u32 shakeFlag;
    struct Vec world;
    struct Vec target;
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
    sum -= gCameraOrigin[2];
    screen->z = sum;
}
// confidence=low: configures a contact effect's movement and independent scale ramp.
#define ContactImpactEffect_BeginMotion sub_081C3024
static inline void SetCallback(void (*callback)(void), void (**slot)(void))
{
    *slot = callback;
}

void ContactImpactEffect_BeginMotion(struct ContactImpactEffect *input, struct Vec *source,
                                     struct Vec *destination, u32 time, u32 trackMode,
                                     u32 rampDuration, s32 rampFrom, u32 rampTo)
{
    struct Vec *from = source;
    struct Vec *to = destination;
    u32 duration = time;
    struct ContactImpactEffect *effect = input;
    if (!effect)
        return;
    if (from) {
        if (!effect->coordinateMode)
            FromScreen(&effect->world, from, 0);
        else
            *&effect->world = *from;
    } else {
        FromScreen(&effect->world, &effect->screen, 512);
    }
    if (!effect->coordinateMode) {
        FromScreen(&effect->target, to, 0);
    } else if (effect->coordinateMode == 1) {
        *&effect->target = *to;
    }
    ToScreen(&effect->screen, &effect->world);
    OrbitMotion_BeginDistanceWave(effect->motion, &effect->world, &effect->target, duration,
                                  trackMode);
    if (rampFrom < 0)
        effect->rampFrom = (s8)effect->byte3C;
    else
        effect->rampFrom = rampFrom;
    effect->rampTo = rampTo;
    effect->shakeThreshold = rampDuration;
    effect->byte3C = effect->rampFrom;
    effect->byte3D = effect->rampFrom;
    effect->flags &= ~1;
    SetCallback(ContactImpactEffect_UpdateContact, &effect->update);
    effect->shakeTimer = 0;
    effect->shakeFlag = 0;
}
