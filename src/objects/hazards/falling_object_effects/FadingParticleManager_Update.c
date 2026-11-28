#include "global.h"
#include "engine/ram.h"
#include "objects/hazards/fading_particle_group.h"
void FadingParticleGroup_Update(struct FadingParticleGroup *);
u32 FadingParticleManager_Update(struct FadingParticleManager *manager)
{
    s32 i;

    for (i = 0; i < 6; i++) {
        struct FadingParticleGroup *group =
            &((struct FadingParticleManager *)gFadingParticleManager)->groups[i];
        if (group->active)
            FadingParticleGroup_Update(group);
    }
    manager->frame++;
    return 0;
}
