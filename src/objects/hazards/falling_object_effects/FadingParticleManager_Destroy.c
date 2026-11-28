#include "objects/hazards/fading_particle_group.h"
#include "engine/task_manager.h"
#include "engine/ram.h"
void Effect_Remove(void *);
u32 FadingParticleManager_Destroy(struct FadingParticleManager *manager)
{
    s32 i, j;

    for (i = 0; i < 6; i++) {
        struct FadingParticleGroup *group = &manager->groups[i];
        if (group->active) {
            for (j = 0; j < group->count; j++)
                Effect_Remove(&group->particles[j].motion.sprite);
        }
    }
    Task_SetManager(NULL, &gFadingParticleManager);
    return 0;
}
