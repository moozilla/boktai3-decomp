#include "monsters/enemy_particle.h"
#include "engine/ram.h"
void SceneOverlayEntry_Init(struct EnemyParticlePool *, struct EnemyParticle *, s32);
u32 EnemyAttackParticle_InitPool(struct EnemyParticlePool *a)
{
    struct EnemyParticle *e;
    s32 i;
    gEnemyParticlePool = a;
    a->mask = 0;
    e = a->slots;
    i = 0;
    do {
        SceneOverlayEntry_Init(a, e, i);
        i++;
        e++;
    } while (i <= 0xF);
    return 0;
}
