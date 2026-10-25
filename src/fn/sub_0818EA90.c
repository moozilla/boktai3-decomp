#include "global.h"
#include "game_state.h"

// MATCHING_CONCESSION: register-pinning

/* Partial views of the three embedded Armor parts. */
struct ArmorLaunchPosition {
    s16 x, y, z, pad;
};
struct ArmorLaunchPart {
    u8 unk_00[0x1C];
    struct ArmorLaunchPosition position;
    u8 unk_24[0x119 - 0x24];
    u8 flags;
    u8 unk_11A[4];
    u8 active, action;
    u8 unk_120[0x134 - 0x120];
    struct ArmorLaunchPosition target;
    u8 unk_13C[0x1E4 - 0x13C];
};
struct ArmorLaunchState {
    u8 flags, unk_01[4], active, action;
    u8 unk_07[0x1E4 - 7];
};
struct ArmorLaunchPositionSlot {
    struct ArmorLaunchPosition position;
    u8 unk_08[0x1E4 - 8];
};
struct ArmorLaunchGroup {
    struct ArmorLaunchPart parts[3];
    struct ArmorLaunchPosition position;
    u8 unk_5B4[0x5C3 - 0x5B4];
    u8 count, activeCount;
};
extern const s16 gUnk_086149C4[];
s32 Math_Atan2(s32, s32);

/* Convert the sine-table product to an integer, rounding toward zero. */
static inline s32 ArmorLaunch_Scale(s32 value)
{
    return value >= 0 ? value >> 12 : -((-value) >> 12);
}

void Armor_QueueFallingParts(void *arg0)
{
    struct ArmorLaunchGroup *group = arg0;
    s32 x, y, z, stepX, targetY, stepZ, angle;
    register s32 i asm("r8");
    struct ArmorLaunchState *state;
    struct ArmorLaunchPositionSlot *position;
    struct ArmorLaunchPart *part;
    u8 *activeCount;
    x = gUnk_02000710->positionX - group->position.x;
    targetY = gUnk_02000710->positionY + 128;
    z = gUnk_02000710->positionZ - group->position.z;
    y = gUnk_02000710->positionY + 2688;
    angle = Math_Atan2(x, z);
    stepX = ArmorLaunch_Scale(gUnk_086149C4[(angle + 64) & 255] * 224);
    stepZ = ArmorLaunch_Scale(gUnk_086149C4[angle & 255] * 224);
    x = gUnk_02000710->positionX;
    z = gUnk_02000710->positionZ;
    if (group->count > 1) {
        x -= stepX;
        z -= stepZ;
    }
    i = 0;
    if (i >= group->count)
        return;
    activeCount = &group->activeCount;
    state = (struct ArmorLaunchState *)&group->parts[0].flags;
    position = (struct ArmorLaunchPositionSlot *)&group->parts[0].position;
    part = group->parts;
    do {
        if (!state->active) {
            position->position.x = x;
            position->position.y = y;
            position->position.z = z;
            part->target.x = x;
            part->target.y = targetY;
            part->target.z = z;
            x += stepX;
            y += 256;
            z += stepZ;
            {
                s32 one = 1;
                state->active = one;
            }
            state->action = 6;
            state->flags = 1 | state->flags;
            (*activeCount)++;
        }
        state++;
        position++;
        part++;
        i++;
    } while (i < group->count);
}
