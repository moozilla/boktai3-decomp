#include "effects/particle_groups/particle_group.h"
#include "engine/shared/ram.h"
#include "engine/shared/task_manager.h"
void Effect_Remove(void *);
u32 ParticleManager_Kill(struct ParticleGroupManager *manager)
{
    s32 i = 0;
    do {
        struct ParticleGroup *group = &manager->groups[i];
        u32 t = group->active;
        s32 next = i + 1;
        if (t != 0) {
            u32 *sprite = &group->particles[0].flags;
            struct ParticleGroupParticle *particle = group->particles;
            s32 k = 7;
            do {
                if (particle->active)
                    Effect_Remove(sprite);
                sprite = (void *)((u8 *)sprite + sizeof(*particle));
                particle++;
                k--;
            } while (k >= 0);
        }
        i = next;
    } while (i <= 5);
    Task_SetManager(NULL, &gParticleGroupManager);
    return 0;
}
