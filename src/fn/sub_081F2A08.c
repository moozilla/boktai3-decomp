// MATCHING_CONCESSION: register-pinning
#include "sprite_trail.h"
#include "background_controller.h"
struct Camera {
    u8 unknown0[0x44];
    s32 x;
};
void Vector3_CopyComponents(struct TrailVector *, struct TrailVector *);
s32 Sprite_LinkToList(void *, u32);
void AnimData_SelectPaletteBank(void *, int);
s32 SpriteTrail_CreateMotionEffect(struct SpriteTrailPool *pool, struct TrailVector *start,
                                   struct TrailVector *end, u32 style, s32 argA, s32 argB)
{
    s32 index;
    struct SpriteTrail *p = MotionEffectPool_FindFreeEntry(pool, &index);
    register struct TrailVector *renderOrigin asm("sl");
    struct TrailVector *position;
    if (!p)
        return -1;
    p->state = 0;
    p->entered = 1;
    p->frame = 0;
    position = &p->position;
    Vector3_CopyComponents(position, start);
    Vector3_CopyComponents(&p->target, end);
    p->style = style;
    p->velocity.x = (end->x - start->x) >> 4;
    p->velocity.y = (end->y - start->y) >> 4;
    p->velocity.z = (end->z - start->z) >> 4;
    p->collisionArg0 = argA;
    p->collisionArg1 = argB;
    p->historyIndex = 0;
    {
        struct TrailVector *origin = position;
        s32 i;
        struct ContactSwarmSprite *head, *tail, *sprite;
        struct TrailVector *history;
        head = p->head;
        tail = p->tail;
        renderOrigin = origin;
        sprite = head;
        history = p->history;
        for (i = 3; i >= 0;) {
            Vector3_CopyComponents(history, origin);
            sprite->position.x = (p->position.x >> 12) - gSceneCamera->x;
            {
                s32 y = origin->y;
                s32 z = origin->z;
                sprite->position.y = ((z + y) >> 12) + 96;
            }
            sprite->position.z = origin->z >> 12;
            sprite->flags &= ~1;
            Sprite_LinkToList(head, 0);
            history++;
            i--;
            sprite++;
            head++;
        }
        origin = renderOrigin;
        i = 2;
        sprite = tail;
        for (; i >= 0;) {
            sprite->position.x = (p->position.x >> 12) - gSceneCamera->x;
            {
                s32 y = origin->y;
                s32 z = origin->z;
                sprite->position.y = ((z + y) >> 12) + 96;
            }
            sprite->position.z = origin->z >> 12;
            sprite->flags &= ~1;
            Sprite_LinkToList(tail, 0);
            i--;
            sprite++;
            tail++;
        }
    }
    AnimData_SelectPaletteBank(p->graphics, 485);
    SpriteTrail_InitRenderCollider(p);
    SpriteTrail_InitCollider(p);
    pool->activeMask |= 1 << index;
    return 0;
}
