#include "global.h"
#include "engine/ram.h"
#include "objects/hazards/fading_particle_group.h"
s32 FadingParticleManager_Initialize(struct FadingParticleManager *, void *);
void FadingParticleManager_Update(void);
void FadingParticleManager_Destroy(void);

void *FadingParticleManager_Create(void *a)
{
    void *p;
    if (gFadingParticleManager != 0)
        return (void *)gFadingParticleManager;
    p = Task_Create(10, sizeof(struct FadingParticleManager));
    if (p != 0) {
        Task_SetCallbacks(p, (void (*)(void *))FadingParticleManager_Update,
                          (void (*)(void *))FadingParticleManager_Destroy);
        if (FadingParticleManager_Initialize(p, a) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
