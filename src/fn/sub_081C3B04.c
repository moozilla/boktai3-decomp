#include "global.h"
#include "engine.h"
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
    screen[0] = screen[0] - gCameraOrigin[0] + 120;
    screen[1] = sy - gCameraOrigin[1] + 80;
    screen[2] = depth - gCameraOrigin[2];
}

extern const u16 gUnk_0203B400[];
s32 sub_08249350(s32, s32);
static inline void ApplyOffset(u16 *field, s32 random, s32 range)
{
    s32 offset = sub_08249350(random >> 4, range);
    *field += offset;
}
// confidence=low: projects the fixed world position and adds a growing random shake.
#define ContactImpactEffect_UpdateShakingPosition sub_081C3B04
void ContactImpactEffect_UpdateShakingPosition(struct ContactImpactEffect *effect)
{
    s32 *timer;
    s32 range;
    const u16 *table;
    u32 *seed;
    u16 index;
    u32 mask;
    s32 random;
    Project(&effect->screenX, (struct Vec *)&effect->worldX);
    timer = &effect->shakeTimer;
    if (*timer >= effect->shakeThreshold)
        effect->shakeFlag = 1;
    range = *timer >> 2;
    table = gUnk_0203B400;
    seed = &gRandomIndex;
    index = *seed + 1;
    mask = 0x3FF;
    index &= mask;
    *seed = index;
    random = table[index];
    ApplyOffset(&effect->screenX, random, range);
    index = (index + 1) & mask;
    *seed = index;
    random = table[index];
    ApplyOffset(&effect->screenY, random, range);
    ++*timer;
}
