#include "tether_actor.h"
#ifdef NONMATCHING
// fuzzy: 77.3%
// Packed endpoint caching and register allocation differ; no exact credit.
static inline s32 scaleOffset(s32 value)
{
    if (value >= 0)
        return value >> 12;
    return -((-value) >> 12);
}
void TetherActor_UpdateSegments(struct TetherActor *input)
{
    struct TetherActor *actor = input;
    struct TetherPlayer **globals = gUnk_02000580;
    struct TetherPlayer *player = globals[0];
    u32 mode = player->tetherMode;
    struct TetherPlayer **slots = globals;
    struct TetherSegment *segment;
    struct ContactPackedVector start, end;
    s32 x;
    s32 z;
    s32 offsetX, offsetZ;
    u32 angle;
    s32 priority;
    s32 index;
    s32 accumulatedX, accumulatedY;
    if (mode != 6) {
        u32 hidden = 1;
        segment = actor->segments;
        index = 6;
        do {
            segment->flags |= hidden;
            --index;
            ++segment;
        } while (index >= 0);
        return;
    }
    angle = ((player->heading + 1) & 7) * 32;
    x = gUnk_086149C4[(angle + 64) & 255];
    z = gUnk_086149C4[angle & 255];
    if (player->side) {
        offsetX = scaleOffset(20 * (z - x));
        offsetZ = scaleOffset(20 * (x + z));
    } else {
        offsetX = scaleOffset(20 * (x + z));
        offsetZ = scaleOffset(20 * (z - x));
    }
    player = slots[0];
    start.x = player->position.x + offsetX;
    start.y = player->position.y + 220;
    start.z = player->position.z + offsetZ;
    angle = ((actor->heading + 5) & 7) * 32;
    end.x = actor->position.x + scaleOffset(100 * gUnk_086149C4[(angle + 64) & 255]);
    end.y = actor->position.y - 50;
    end.z = actor->position.z + scaleOffset(100 * gUnk_086149C4[angle & 255]);
    priority = -2;
    if (slots[0]->heading == 4)
        priority = -20;
    segment = actor->segments;
    index = 0;
    {
        s32 startX = start.x;
        s32 startY = start.y;
        s32 endX = end.x;
        s32 nextX = endX;
        s32 endY = end.y;
        s32 nextY = endY;
        do {
            s32 count = index + 1;
            s32 remaining = 7 - index;
            segment->position.x = (nextX + startX * remaining) >> 3;
            segment->position.y = (nextY + startY * remaining) >> 3;
            segment->position.z = (end.z * count + start.z * remaining) >> 3;
            segment->priority = priority;
            segment->flags &= ~1;
            nextX += endX;
            nextY += endY;
            index = count;
            ++segment;
        } while (index <= 6);
    }
}

#else
INCLUDE_ASM("asm/nonmatching", TetherActor_UpdateSegments);
#endif
