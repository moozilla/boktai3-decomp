#include "armor_part_queue.h"
#include "game_state.h"

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
    s32 i;
    struct ArmorLaunchState *state;
    struct ArmorLaunchPositionSlot *position;
    struct ArmorLaunchPart *part;
    u8 *activeCount;
    u32 active;
    active = 1;
    x = gGameState->positionX - group->position.x;
    targetY = gGameState->positionY + 128;
    z = gGameState->positionZ - group->position.z;
    y = gGameState->positionY + 2688;
    angle = Math_Atan2(x, z);
    stepX = ArmorLaunch_Scale(gUnk_086149C4[(angle + 64) & 255] * 224);
    stepZ = ArmorLaunch_Scale(gUnk_086149C4[angle & 255] * 224);
    x = gGameState->positionX;
    z = gGameState->positionZ;
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
            ARMOR_QUEUE_PART_ACTION(state, 6, active, 1, activeCount);
        }
        state++;
        position++;
        part++;
        i++;
    } while (i < group->count);
}
