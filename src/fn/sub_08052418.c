#include "stacked_actor.h"
static inline u16 StackedSegment_GetOffset(u32 limit, u32 height, u32 offset)
{
    return (limit - height) * 171 + offset;
}
void StackedActor_UpdateSegments(struct StackedActor *actor)
{
    u16 *height = &actor->height;
    if (*height) {
        if (*height <= 2) {
            u16 *worldY;
            actor->segments[0].flags &= ~1;
            worldY = (u16 *)&actor->world.y;
            actor->segments[0].position.y = StackedSegment_GetOffset(2, *height, 1026) + *worldY;
            actor->segments[1].flags |= 1;
            actor->segments[2].flags |= 1;
            {
                u32 frame = actor->baseFrame;
                struct StackedActorSprite *sprite = &actor->sprite;
                sprite->frame = frame;
            }
        } else if (*height <= 4) {
            u16 *worldY;
            actor->segments[0].flags &= ~1;
            worldY = (u16 *)&actor->world.y;
            actor->segments[0].position.y = 1026 + *worldY;
            actor->segments[1].flags &= ~1;
            actor->segments[1].position.y = StackedSegment_GetOffset(4, *height, 684) + *worldY;
            actor->segments[2].flags |= 1;
            {
                u32 frame = actor->baseFrame;
                struct StackedActorSprite *sprite = &actor->sprite;
                sprite->frame = frame;
            }
        } else if (*height <= 6) {
            u16 *worldY;
            actor->segments[0].flags &= ~1;
            worldY = (u16 *)&actor->world.y;
            actor->segments[0].position.y = 1026 + *worldY;
            actor->segments[1].flags &= ~1;
            actor->segments[1].position.y = 684 + *worldY;
            actor->segments[2].flags &= ~1;
            actor->segments[2].position.y = StackedSegment_GetOffset(6, *height, 342) + *worldY;
            {
                u32 frame = actor->baseFrame;
                struct StackedActorSprite *sprite = &actor->sprite;
                sprite->frame = frame;
            }
            actor->sprite.renderMode = 3;
        } else {
            u16 *worldY;
            actor->segments[0].flags &= ~1;
            worldY = (u16 *)&actor->world.y;
            actor->segments[0].position.y = 1026 + *worldY;
            actor->segments[1].flags &= ~1;
            actor->segments[1].position.y = 684 + *worldY;
            actor->segments[2].flags &= ~1;
            actor->segments[2].position.y = 342 + *worldY;
            {
                u32 frame = actor->baseFrame;
                struct StackedActorSprite *sprite = &actor->sprite;
                frame += 8;
                sprite->frame = frame;
            }
            actor->sprite.flags |= 0x200;
            actor->sprite.renderMode = 1;
        }
    } else {
        actor->segments[0].flags |= 1;
        actor->segments[1].flags |= 1;
        actor->segments[2].flags |= 1;
    }
}
