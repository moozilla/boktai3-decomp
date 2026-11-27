#include "effects/particles/particle_group.h"
#include "engine/ram.h"
void Effect_Remove(void *);

void ImpactParticleManager_ClearActiveGroups(void)
{
    struct ParticleGroupManager *manager = gParticleGroupManager;
    if (manager != 0) {
        s32 i = 0;
        do {
            struct ParticleGroup *group = &manager->groups[i];
            u32 t = group->active;
            s32 next = i + 1;
            if (t != 0) {
                struct ParticleGroupParticle *particle = group->particles;
                u32 *sprite = &group->particles[0].flags;
                s32 k = 7;
                do {
                    if (particle->active) {
                        Effect_Remove(sprite);
                        particle->active = 0;
                    }
                    particle++;
                    sprite = (void *)((u8 *)sprite + sizeof(*particle));
                    k--;
                } while (k >= 0);
                group->active = 0;
            }
            i = next;
        } while (i <= 5);
    }
}
