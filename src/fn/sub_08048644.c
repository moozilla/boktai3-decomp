#include "tether_actor.h"
#define TetherActor_UpdateSegments sub_08048644
#ifdef NONMATCHING
// fuzzy: 77.3%
// Packed endpoint caching and register allocation differ; no exact credit.
// MATCHING_CONCESSION: register-pinning
static inline s32 scaleOffset(s32 value)
{
    if (value >= 0)
        return value >> 12;
    return -((-value) >> 12);
}
void TetherActor_UpdateSegments(struct TetherActor *input)
{
    register struct TetherActor *actor asm("r6") = input;
    struct TetherPlayer **globals = gUnk_02000580;
    struct TetherPlayer *player = globals[0];
    register u32 mode asm("r1") = player->tetherMode;
    register struct TetherPlayer **slots asm("r8") = globals;
    register struct TetherSegment *segment asm("r4");
    struct ContactPackedVector start, end;
    s32 x;
    register s32 z asm("r2");
    s32 offsetX, offsetZ;
    u32 angle;
    register s32 priority asm("r9");
    register s32 index asm("r1");
    s32 accumulatedX, accumulatedY;
    if (mode != 6) {
        register u32 hidden asm("r2") = 1;
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
    register s32 startX asm("r10") = start.x;
    register s32 startY asm("r8") = start.y;
    register s32 endX asm("r12") = end.x;
    register s32 nextX asm("r6") = endX;
    s32 endY = end.y;
    register s32 nextY asm("r5") = endY;
    do {
        register s32 count asm("r3") = index + 1;
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
INCLUDE_ASM("asm/nonmatching", sub_08048644);
#endif
