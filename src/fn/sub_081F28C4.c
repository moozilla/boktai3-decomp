#include "global.h"
// confidence=low: SpriteTrail_CreateMotionEffect without the velocity: the end point goes to the
// slot that function uses for velocity (0x54), and the trail sprites start at the start point.
// MATCHING_CONCESSION: register-pinning
struct Vec {
    s32 x, y, z, pad;
};
struct Sprite {
    u32 flags;
    u8 pad4[0x18];
    s16 x, y, z;
    u8 pad22[0xa];
};
struct Trail {
    u8 active, phase, pad2[2];
    struct Vec position, history[4], velocity, target;
    u32 style, argA, argB, age, frame;
    u8 pad88[0x80];
    struct Sprite head[4], tail[3];
    u8 animation[0x20];
};
struct Pool {
    u8 pad0[0x18];
    u32 mask;
};
struct Camera {
    u8 pad0[0x44];
    s32 x;
};
extern struct Camera *gUnk_020004B8;
struct Trail *MotionEffectPool_FindFreeEntry(struct Pool *, s32 *);
void Vector3_CopyComponents(struct Vec *, struct Vec *);
void Sprite_LinkToList(void *, int);
void AnimData_SelectPaletteBank(void *, int);
void SpriteTrail_InitRenderCollider(void *);
void SpriteTrail_InitCollider(void *);
s32 SpriteTrail_CreateFixedEffect(struct Pool *pool, struct Vec *start, struct Vec *end, u32 style,
                                  s32 argA, s32 argB)
{
    s32 index, i;
    struct Trail *p = MotionEffectPool_FindFreeEntry(pool, &index);
    register struct Vec *position;
    struct Sprite *head, *tail, *sprite;
    struct Vec *history;
    register struct Vec *saved asm("sl");
    if (!p)
        return -1;
    p->phase = 0;
    p->active = 1;
    p->frame = 0;
    position = &p->position;
    Vector3_CopyComponents(position, start);
    Vector3_CopyComponents(&p->velocity, end);
    p->style = style;
    p->argA = argA;
    p->argB = argB;
    p->age = 0;
    head = p->head;
    tail = p->tail;
    saved = position;
    sprite = head;
    history = p->history;
    for (i = 3; i >= 0;) {
        Vector3_CopyComponents(history, position);
        sprite->x = (p->position.x >> 12) - gUnk_020004B8->x;
        {
            s32 y = position->y;
            s32 z = position->z;
            sprite->y = ((z + y) >> 12) + 96;
        }
        sprite->z = position->z >> 12;
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
        sprite->x = (p->position.x >> 12) - gUnk_020004B8->x;
        {
            s32 y = position->y;
            s32 z = position->z;
            sprite->y = ((z + y) >> 12) + 96;
        }
        sprite->z = position->z >> 12;
        sprite->flags &= ~1;
        Sprite_LinkToList(tail, 0);
        i--;
        sprite++;
        tail++;
    }
    AnimData_SelectPaletteBank(p->animation, 485);
    SpriteTrail_InitRenderCollider(p);
    SpriteTrail_InitCollider(p);
    pool->mask |= 1 << index;
    return 0;
}
