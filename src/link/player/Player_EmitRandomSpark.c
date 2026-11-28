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

void Player_EmitRandomSpark(void *p, struct SceneVec *in, u32 angle, u32 extra)
{
    struct DirectionalParticleVector a, b;
    s32 k;
    a.x = (ParticleRandom_ReadDirectSample(gRandomTable) % 32) + in->x - 15;
    a.y = (ParticleRandom_ReadDirectSample(gRandomTable) % 32) + in->y - 15;
    a.z = (ParticleRandom_ReadDirectSample(gRandomTable) % 32) + in->z - 15;
    k = (ParticleRandom_ReadDirectSample(gRandomTable) % 4) + 3;
    b.x = -scale(gUnk_086149C4[(angle + 64) & 255] * k);
    b.y = 0;
    b.z = -scale(gUnk_086149C4[angle & 255] * k);
    DirectionalEffect_AddParticle(p, &a, &b, extra);
}
