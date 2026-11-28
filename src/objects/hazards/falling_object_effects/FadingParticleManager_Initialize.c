#include "global.h"
#include "engine/ram.h"
#include "objects/hazards/fading_particle_group.h"
void *AnimData_FindEntry(int);
void Effect_Initialize(void *, const void *, int);
s32 FadingParticleManager_Initialize(struct FadingParticleManager *manager)
{
    s32 i, j;

    gFadingParticleManager = manager;
    manager->graphics = AnimData_FindEntry(0x1c1a);
    manager->animations = ScriptProc_Start(0x922e, 0xd1b8);
    for (i = 0; i < 6; i++) {
        struct FadingParticleGroup *group = &manager->groups[i];
        group->active = 0;
        for (j = 0; j < 8; j++) {
            struct FadingParticle *particle = &group->particles[j];
            Effect_Initialize(&particle->motion.sprite, (void *)manager->graphics, 0);
            Anim_SetSequence(&particle->motion.anim, manager->animations, 3, 0, 0);
        }
    }
    manager->frame = 0;
    return 0;
}
