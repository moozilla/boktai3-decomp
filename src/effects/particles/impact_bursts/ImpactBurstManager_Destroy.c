#include "effects/particles/animated_particle.h"
#include "engine/ram.h"
#include "engine/task_manager.h"
void Sprite_Remove(void *);
u32 ImpactBurstManager_Destroy(struct AnimatedParticleManager *manager)
{
    struct DisplaySequenceSprite *sprite = &manager->pieces[0].sprite;
    struct AnimatedParticle *particle = manager->pieces;
    s32 i = 0x1f;
    do {
        if (particle->active)
            Sprite_Remove(sprite);
        sprite = (void *)((u8 *)sprite + sizeof(*particle));
        particle++;
        i--;
    } while (i >= 0);
    Task_SetManager(NULL, &gAnimatedParticleManager);
    return 0;
}
