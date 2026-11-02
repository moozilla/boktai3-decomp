#include "spiral_burst.h"

static inline void FirstSpiralParticle(struct SpiralBurst *burst, void **drawable,
                                       struct SpiralParticleMotion **motion)
{
    struct SpiralParticle *first = burst->particles;
    *drawable = first;
    *motion = &first->frame.motion;
}

static inline void GetSpiralUpdateState(struct SpiralBurst *burst, u16 **frame,
                                        struct SpiralBurst **owner)
{
    *owner = burst;
    *frame = &burst->timer;
}

void SpiralBurst_UpdateParticles(struct SpiralBurst *p)
{
    u32 alive = 0;
    s32 i;
    void *sprite;
    u32 off;
    struct SpiralParticleMotion *motion;
    struct SpiralBurst *owner;
    u16 *initialFrame;
    u16 *timer;
    GetSpiralUpdateState(p, &initialFrame, &owner);
    if (*initialFrame <= 11)
        Collider_Refresh(owner->collider);
    timer = &p->timer;
    off = 0;
    FirstSpiralParticle(owner, &sprite, &motion);
    i = 7;
    do {
        if (*timer >= motion->lifetime) {
            u8 *q = (u8 *)p->particles;
            *(u32 *)(q + off) |= 1;
        } else {
            alive = 1;
            motion->angle += motion->angularSpeed;
            motion->position.world.x =
                p->origin.x +
                SpiralBurst_ScaleOffset(motion->radius * gUnk_086149C4[(motion->angle + 64) & 255]);

            motion->position.world.z =
                p->origin.z +
                SpiralBurst_ScaleOffset(motion->radius * gUnk_086149C4[motion->angle]);
            if (*timer >= motion->riseDelay) {
                if ((*timer & 7) == 0 && motion->angularSpeed > 2)
                    motion->angularSpeed--;
                motion->position.world.y += motion->riseRate;
            }
            if (motion->animationFrame == 0)
                SpriteDrawable_SelectTileFrame(sprite, p->resource, 12);
            else if (motion->animationFrame == 4) {
                u32 q = off + (u32) & ((struct SpiralBurst *)0)->particles;
                SpriteDrawable_SelectTileFrame((void *)((u8 *)p + q), p->resource, 13);
            }
            motion->animationFrame++;
            if (motion->animationFrame > 7)
                motion->animationFrame = 0;
        }
        off += sizeof(struct SpiralParticle);
        sprite = (u8 *)sprite + sizeof(struct SpiralParticle);
        motion = (void *)((u8 *)motion + sizeof(struct SpiralParticle));
    } while (--i >= 0);
    p->timer++;
    if (!alive)
        Task_Kill(p);
}
