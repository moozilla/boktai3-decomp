#include "effects/healing/healing_effect.h"
#include "engine/math/monster_particle_random.h"
#include "engine/engine.h"

s32 Mod(s32, s32);

void HealthRestoreEffect_InitRisingParticle(struct HealingParticle *particle)
{
    const u16 *table;

    particle->drawable.flags &= ~1;
    SpriteDrawable_SelectTileFrame(&particle->drawable, particle->resource, 6);
    table = gRandomTable;
    particle->drawable.position.x = Mod(HealingEffect_Random(table), 16) + 8;
    particle->drawable.position.y = Mod(HealingEffect_Random(table), 16) + 24;
    particle->drawable.position.z = 0;
    particle->velocity.x = 0;
    particle->velocity.y = -(Mod(HealingEffect_Random(table), 2) + 2);
    particle->velocity.z = 0;
    particle->active = 1;
    particle->age = 0;
    particle->update = FlickerParticle_UpdateVerticalMotion;
}
