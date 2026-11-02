// MATCHING_CONCESSION: register-pinning
#include "spiral_burst.h"

static inline void FirstSpiralParticle(struct SpiralBurst *burst, void **drawable,
                                       struct SpiralParticleMotion **motion)
{
    struct SpiralParticle *first = burst->particles;
    *drawable = first;
    *motion = &first->frame.motion;
}

void SpiralBurst_UpdateParticles(struct SpiralBurst *p)
{
    u32 alive = 0;
    s32 i;
    register void *sprite;
    u32 off;
    struct SpiralParticleMotion *motion;
    u16 *timer;
    {
        register u32 offset asm("r1") = (u32) & ((struct SpiralBurst *)0)->timer;
        register u16 *address;
        address = (void *)((u8 *)p + offset);
        if (*address <= 11)
            Collider_Refresh(p->collider);
    }
    asm("" : "+r"(p)::"r4");
    timer = &p->timer;
    off = 0;
    FirstSpiralParticle(p, &sprite, &motion);
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
