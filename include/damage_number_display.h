#ifndef GUARD_DAMAGE_NUMBER_DISPLAY_H
#define GUARD_DAMAGE_NUMBER_DISPLAY_H
#include "contact_actor.h"
/* Tentative floating-number family: four groups, each with four 0x28 glyphs. */
struct DamageNumberGlyph {
    u32 unknown0;
    u8 unknown4[11];
    u8 priority;
    u8 unknown10[8];
    s16 x, y;
    u8 unknown1C[12];
};
struct DamageNumberGroup {
    u32 sourceKey;
    u8 priority, phaseFrame, phase, digitCount;
    s16 anchorY, raisedY;
    struct ContactPosition position;
    struct DamageNumberGlyph digits[4];
};
struct DamageNumberDisplay {
    u8 task[0x18];
    u32 activeMask;
    void *resource;
    struct DamageNumberGroup groups[4];
};
extern struct DamageNumberDisplay *gDamageNumberDisplay;
static inline s32 DamageNumber_ProjectScale(s32 value)
{
    if (value >= 0)
        return value >> 5;
    return -((-value) >> 5);
}
extern u16 gCameraOrigin[];
static inline void DamageNumber_ProjectPosition(struct ContactPosition *screen,
                                                const struct ContactPosition *world)
{
    s32 x = world->x;
    s32 z = world->z;
    s32 sum, vertical, y, depth;
    s32 scaledSum;
    screen->x = DamageNumber_ProjectScale(3 * (x - z));
    scaledSum = DamageNumber_ProjectScale(3 * (x + z));
    sum = scaledSum;
    vertical = DamageNumber_ProjectScale(3 * world->y);
    y = sum - vertical;
    depth = sum + vertical;
    screen->x = (u16)screen->x - gCameraOrigin[0] + 120;
    screen->y = y - gCameraOrigin[1] + 80;
    screen->z = depth - gCameraOrigin[2];
}
#endif
