#include "sprite_trail.h"
#include "background_controller.h"
// confidence=low: SpriteTrail_CreateMotionEffect without the velocity: the end point goes to the
// slot that function uses for velocity (0x54), and the trail sprites start at the start point.
// MATCHING_CONCESSION: register-pinning
struct Camera {
    u8 pad0[0x44];
    s32 x;
};
void Vector3_CopyComponents(struct TrailVector *, struct TrailVector *);
s32 Sprite_LinkToList(void *, u32);
void AnimData_SelectPaletteBank(void *, int);
s32 SpriteTrail_CreateFixedEffect(struct SpriteTrailPool *pool, struct TrailVector *start,
                                  struct TrailVector *end, u32 style, s32 argA, s32 argB)
{
    s32 index, i;
    struct SpriteTrail *p = MotionEffectPool_FindFreeEntry(pool, &index);
    register struct TrailVector *position;
    struct ContactSwarmSprite *head, *tail, *sprite;
    struct TrailVector *history;
    register struct TrailVector *saved asm("sl");
    if (!p)
        return -1;
    p->state = 0;
    p->entered = 1;
    p->frame = 0;
    position = &p->position;
    Vector3_CopyComponents(position, start);
    Vector3_CopyComponents(&p->velocity, end);
    p->style = style;
    p->collisionArg0 = argA;
    p->collisionArg1 = argB;
    p->historyIndex = 0;
    head = p->head;
    tail = p->tail;
    saved = position;
    sprite = head;
    history = p->history;
    for (i = 3; i >= 0;) {
        Vector3_CopyComponents(history, position);
        sprite->position.x = (p->position.x >> 12) - gSceneCamera->x;
        {
            s32 y = position->y;
            s32 z = position->z;
            sprite->position.y = ((z + y) >> 12) + 96;
        }
        sprite->position.z = position->z >> 12;
        sprite->flags &= ~1;
        Sprite_LinkToList(head, 0);
        history++;
        i--;
        sprite++;
        head++;
    }
    position = saved;
    i = 2;
    sprite = tail;
    for (; i >= 0;) {
        sprite->position.x = (p->position.x >> 12) - gSceneCamera->x;
        {
            s32 y = position->y;
            s32 z = position->z;
            sprite->position.y = ((z + y) >> 12) + 96;
        }
        sprite->position.z = position->z >> 12;
        sprite->flags &= ~1;
        Sprite_LinkToList(tail, 0);
        i--;
        sprite++;
        tail++;
    }
    AnimData_SelectPaletteBank(p->graphics, 485);
    SpriteTrail_InitRenderCollider(p);
    SpriteTrail_InitCollider(p);
    pool->activeMask |= 1 << index;
    return 0;
}
