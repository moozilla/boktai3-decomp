#include "effects/fading_particle_groups/fading_particle_group.h"
#include "engine/shared/task_manager.h"
#include "engine/shared/ram.h"
void Effect_Remove(void *);
u32 FadingParticleManager_Destroy(struct FadingParticleManager *manager)
{
    s32 i = 0;
    do {
        struct FadingParticleGroup *group = &manager->groups[i];
        u32 t = group->active;
        s32 next = i + 1;
        if (t != 0) {
            s32 j = 0;
            struct SpriteDrawable *sprite;
            if (j < group->count) {
                sprite = &group->particles[0].motion.sprite;
                do {
                    Effect_Remove(sprite);
                    sprite = (void *)((u8 *)sprite + sizeof(struct FadingParticle));
                    j++;
                } while (j < group->count);
            }
        }
        i = next;
    } while (i <= 5);
    Task_SetManager(NULL, &gFadingParticleManager);
    return 0;
}
