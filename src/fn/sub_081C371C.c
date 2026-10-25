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
static inline void Project(u16 *screen, struct Vec *world)
{
    s32 x = world->x;
    s32 z = world->z;
    s32 sum;
    s32 y;
    s32 sy, depth;
    screen[0] = Scale(x - z);
    sum = Scale(x + z);
    y = Scale(world->y);
    sy = sum - y;
    depth = sum + y;
    screen[0] = screen[0] - gUnk_03005418[0] + 120;
    screen[1] = sy - gUnk_03005418[1] + 80;
    screen[2] = depth - gUnk_03005418[2];
}
void ContactImpactEffect_UpdateInterpolatedPosition(void);
static inline void SetCallback(void (*callback)(void), void (**slot)(void))
{
    *slot = callback;
}

// confidence=low: initializes linear world movement and a scale ramp without a motion track.
#define ContactImpactEffect_BeginInterpolatedPosition sub_081C371C
#ifdef NONMATCHING
// fuzzy: 93.0%
// fix.py best (sweep after the inlined-insn fix)
// Full reconstruction; destination/origin pointer copies and projection registers still differ.
static inline void Project_081C371C(u16 *screen, struct Vec *world)
{
    s32 x = world->x;
    s32 z = world->z;
    s32 sum;
    s32 y;
    s32 sy;
    s32 depth;
    screen[0] = Scale(x - z);
    sum = Scale(x + z);
    y = Scale(world->y);
    sy = sum - y;
    depth = sum + y;
    screen[0] = screen[0] - gUnk_03005418[0] + 120;
    screen[1] = sy - gUnk_03005418[1] + 80;
    screen[2] = depth - gUnk_03005418[2];
}

void ContactImpactEffect_BeginInterpolatedPosition(struct ContactImpactEffect *input,
                                                   struct Vec *source, struct Vec *destination,
                                                   s32 initialScale, u32 finalScale, u32 duration)
{
    struct Vec *from = source;
    s32 rampFrom = initialScale;
    struct ContactImpactEffect *effect = input;
    if (effect) {
        if (!effect->coordinateMode) {
            struct Vec *out = (struct Vec *)(&effect->worldX);
            struct Vec *origin;
            origin = out;
            FromScreen(out, from, 0);
            FromScreen((struct Vec *)(&effect->targetX), destination, 0);
            Project_081C371C(&effect->screenX, origin);
        } else if (effect->coordinateMode == 1) {
            struct Vec *origin = (struct Vec *)(&effect->worldX);
            *origin = *from;
            *((struct Vec *)(&effect->targetX)) = *destination;
            Project_081C371C(&effect->screenX, origin);
        }
        effect->shakeThreshold = duration;
        *((u32 *)(((u8 *)effect) + 0xEC)) = 0;
        if (rampFrom < 0)
            effect->rampFrom = (s8)effect->byte3C;
        else
            effect->rampFrom = rampFrom;
        effect->rampTo = finalScale;
        effect->byte3C = effect->rampFrom;
        effect->byte3D = effect->rampFrom;
        *((u32 *)(((u8 *)effect) + 0x34)) &= ~1;
        SetCallback(ContactImpactEffect_UpdateInterpolatedPosition,
                    (void *)(((u8 *)effect) + 0xF4));
        effect->shakeTimer = 0;
        effect->shakeFlag = 0;
    }
}

#else
INCLUDE_ASM("asm/nonmatching", sub_081C371C);
#endif
