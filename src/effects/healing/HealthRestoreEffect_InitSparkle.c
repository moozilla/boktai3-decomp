#include "effects/healing/healing_effect.h"
#include "engine/math/monster_particle_random.h"
#include "engine/engine.h"

s32 Mod(s32, s32);

void HealthRestoreEffect_InitSparkle(struct HealingParticle *particle)
{
    const u16 *table;

    particle->drawable.flags &= ~1;
    SpriteDrawable_SelectTileFrame(&particle->drawable, particle->resource, 2);
    table = gRandomTable;
    particle->drawable.position.x = Mod(HealingEffect_Random(table), 80) + 24;
    particle->drawable.position.y = ((HealingEffect_Random(table)) & 7) + 8;
    particle->drawable.position.z = 0;
    particle->active = 1;
    particle->age = 0;
    particle->update = FlickerParticle_UpdateStationary;
}
