#include "effects/healing/healing_effect.h"
#include "engine/engine.h"

void HealthBurstEffect_InitParticlePool(struct HealthBurstEffect *effect)
{
    const struct SpriteEffectTileResource *resource = AnimData_FindEntry(0x1C1E);
    s32 i;

    for (i = 0; i < 8; i++) {
        struct HealingParticle *particle = &effect->particles[i];

        effect->particles[i].resource = resource;
        ParticleSprite_Initialize(&particle->drawable, resource, 17);
        Effect_SetOrigin(&particle->drawable, -4, -4);
        SpriteDrawable_SelectTileFrame(&particle->drawable, effect->particles[i].resource, 2);
        Effect_SetPalette(&particle->drawable, 1);
        particle->drawable.priority = 0;
        effect->particles[i].age = 0;
        effect->particles[i].active = 0;
    }
}
