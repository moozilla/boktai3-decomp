#include "global.h"
#include "engine/ram.h"
void *Task_Create(u32, u32);
void Task_SetCallbacks(void *, void (*)(void), void (*)(void));
s32 ImpactParticleManager_Initialize(void *, void *);
void Task_Kill(void *);
void ImpactParticleManager_Update(void);
void ImpactParticleManager_Kill(void);

void *ImpactParticleManager_Create(void *a)
{
    void *p;
    if (gParticleGroupManager != 0)
        return (void *)gParticleGroupManager;
    p = Task_Create(10, 0xabc);
    if (p != 0) {
        Task_SetCallbacks(p, ImpactParticleManager_Update, ImpactParticleManager_Kill);
        if (ImpactParticleManager_Initialize(p, a) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
