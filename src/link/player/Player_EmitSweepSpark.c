#include "global.h"
#include "engine/math/scene_vec.h"
#include "engine/engine_globals.h"
#include "engine/ram.h"
#include "engine/math/monster_particle_random.h"
#include "link/player/directional_particle_vector.h"

extern const s16 gUnk_086149C4[];

void DirectionalEffect_AddParticle(void *, struct DirectionalParticleVector *,
                                   struct DirectionalParticleVector *, u32);

static inline s32 scale(s32 product)
{
    return product >= 0 ? product >> 12 : -((-product) >> 12);
}

void Player_EmitSweepSpark(void *p, struct SceneVec *in, u32 angle, u32 side, u32 index)
{
    struct DirectionalParticleVector a, b;
    u32 t;
    s32 k;
    gRandomIndex = (gRandomIndex + 1) & 0x3FF;
    t = *(gRandomTable + gRandomIndex);
    k = (t & 0x7F) + 0x70;
    a.x = in->x + scale(gUnk_086149C4[(angle + 64) & 255] * k);
    a.y = in->y;
    a.z = in->z + scale(gUnk_086149C4[angle & 255] * k);
    b.x = scale(gUnk_086149C4[angle & 255] * 6);
    b.y = 0;
    b.z = -scale(gUnk_086149C4[(angle + 64) & 255] * 6);
    if (side) {
        b.x = -b.x;
        b.z = -b.z;
    }
    DirectionalEffect_AddParticle(p, &a, &b, index);
}
