// MATCHING_CONCESSION: register-pinning
#include "global.h"
extern u16 gUnk_03005418[];
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
    s32 x = screen->x + (s16)gUnk_03005418[0] - 120;
    s32 y = screen->y + (s16)gUnk_03005418[1] - 80;
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
    screen->x = screen->x - gUnk_03005418[0] + 120;
    screen->y = screen->y - gUnk_03005418[1] + 80;
    screen->z = screen->z - gUnk_03005418[2];
}
// confidence=low: configures a contact effect's movement and independent scale ramp.
#define ContactImpactEffect_BeginMotion sub_081C3024
static inline void SetCallback(void (*callback)(void), void (**slot)(void))
{
    *slot = callback;
}

#ifdef NONMATCHING
// fix.py best (sweep after the inlined-insn fix)
// Full reconstruction; vector pointer lifetimes and projection/ramp register allocation differ.
void ContactImpactEffect_BeginMotion(struct ContactImpactEffect *input, struct Vec *source,
                                     struct Vec *destination, u32 time, u32 trackMode,
                                     u32 rampDuration, s32 rampFrom, u32 rampTo)
{
    struct Vec *from = source;
    struct Vec *to = destination;
    u32 duration = time;
    struct ContactImpactEffect *effect = input;
    register struct Vec *origin asm("sl");
    struct Vec *screen;
    struct Vec *target;
    if (!effect)
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
    } else {
        origin = (struct Vec *)(&effect->worldX);
        screen = (struct Vec *)(&effect->screenX);
        FromScreen(origin, screen, 512);
    }
    if (!effect->coordinateMode) {
        target = (struct Vec *)(&effect->targetX);
        FromScreen(target, to, 0);
    } else {
        target = (struct Vec *)(&effect->targetX);
        {
            typeof(effect->coordinateMode) v1 = effect->coordinateMode;
            if (v1 == 1)
                *target = *to;
        }
    }
    do {
        ToScreen(screen, origin);
    } while (0);
    OrbitMotion_BeginDistanceWave(((u8 *)effect) + 0xA0, origin, target, duration, trackMode);
    if (rampFrom < 0)
        effect->rampFrom = (s8)effect->byte3C;
    else
        effect->rampFrom = rampFrom;
    effect->rampTo = rampTo;
    effect->shakeThreshold = rampDuration;
    effect->byte3C = effect->rampFrom;
    effect->byte3D = effect->rampFrom;
    *((u32 *)(((u8 *)effect) + 0x34)) &= ~1;
    SetCallback(ContactImpactEffect_UpdateContact, (void *)(((u8 *)effect) + 0xF4));
    effect->shakeTimer = 0;
    effect->shakeFlag = 0;
}

#else
INCLUDE_ASM("asm/nonmatching", sub_081C3024);
#endif
