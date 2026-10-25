// MATCHING_CONCESSION: register-pinning
#include "global.h"

#ifdef NONMATCHING
// fuzzy: 85.3%
// fix.py best (sweep after the inlined-insn fix)
// fix.py best version (reprinted C)
// confidence=low: spawns one particle (kind 0/1 picks tile frames) into a 16-slot pool.
// Remaining difference: random-index/table register allocation in the position jitter.
#include "scene_vec.h"

#define gParticleRandomIndex gUnk_03005308
struct PoolParticle {
    u32 flags;
    u8 unknown4[0x14];
    struct SceneVec position;
    u8 unknown20[8];
    u8 active, angle, riseSpeed, expandDelay, animationPhase, angularSpeed, lifetime, state;
    u16 radius;
    u8 kind, pad33;
    s16 velocityX, velocityY, velocityZ, unknown3A;
};
struct ParticlePool {
    u8 unknown0[0x70];
    void *resource;
    struct PoolParticle particles[16];
};
extern u16 gUnk_0203B400[];
extern u32 gUnk_03005308;
extern const s16 gUnk_086149C4[];
void SpriteDrawable_SelectTileFrame(void *, void *, u32);
static inline s32 TruncatePoolOffset(s32 value)
{
    return value >= 0 ? value >> 12 : -(-value >> 12);
}
void sub_081639E4(struct ParticlePool *inPool, struct SceneVec *position, s32 angle, u32 kind)
{
    s32 i;
    struct PoolParticle *p;
    asm("" : "+r"(position));
    for (i = 0; i < 16; i++) {
        if (!inPool->particles[i].active) {
            break;
        }
    }

    if (i < 16) {
        {
            u32 range;
            u32 mask;
            u32 index;
            p = &inPool->particles[i];
            p->flags &= ~1;
            p->position = *position;
            index = gUnk_03005308;
            index++;
            mask = 0x3FF;
            index &= mask;
            {
                u32 random = (*((typeof(gUnk_0203B400) *)0x0203B400))[index];
                s32 x = (u16)p->position.x;
                x -= 63;
                range = 0x7F;
                random &= range;
                p->position.x = x + random;
            }
            index = (index + 1) & mask;
            gUnk_03005308 = index;
            {
                u32 random = (*((typeof(gUnk_0203B400) *)0x0203B400))[index];
                s32 z = (u16)p->position.z;
                z -= 63;
                p->position.z = z + random;
                random &= range;
            }
        }
        p->position.y += 128;
        if (kind != 0) {
            if (kind == 1) {
                if ((i & kind) == 0)
                    SpriteDrawable_SelectTileFrame(p, inPool->resource, 4);
                else
                    SpriteDrawable_SelectTileFrame(p, inPool->resource, 5);
            }
        } else if ((i & 1) == 0)
            SpriteDrawable_SelectTileFrame(p, inPool->resource, 2);
        else
            SpriteDrawable_SelectTileFrame(p, inPool->resource, 3);
        p->velocityX = TruncatePoolOffset(6 * gUnk_086149C4[(angle + 64) & 255]);
        p->velocityZ = TruncatePoolOffset(6 * gUnk_086149C4[angle & 255]);
        p->animationPhase = 0;
        p->state = 1;
        {
            u32 index = (gUnk_03005308 + 1) & 0x3FF;
            gUnk_03005308 = index;
            {
                u32 random =
                    *((u16 *)((index << 1) + ((u32)(*((typeof(gUnk_0203B400) *)0x0203B400)))));
                u32 range = 7;
                p->lifetime = (random & range) + 8;
            }
        }
        p->active = 1;
        p->kind = kind;
    }
}

#else
INCLUDE_ASM("asm/nonmatching", sub_081639E4);
#endif
