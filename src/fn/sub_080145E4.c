#include "global.h"
#include "ram.h"
void *Task_Create(u32, u32);
void Task_SetCallbacks(void *, void (*)(void), void (*)(void));
s32 ParticleManager_Initialize(void *, void *);
void Task_Kill(void *);
void ParticleManager_Update(void);
void ParticleManager_Kill(void);

void *ParticleManager_Create(void *a)
{
    void *p;
    if (gParticleGroupManager != 0)
        return (void *)gParticleGroupManager;
    p = Task_Create(10, 0xabc);
    if (p != 0) {
        Task_SetCallbacks(p, ParticleManager_Update, ParticleManager_Kill);
        if (ParticleManager_Initialize(p, a) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
