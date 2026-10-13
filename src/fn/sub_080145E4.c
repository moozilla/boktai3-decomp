#include "global.h"
extern u32 gUnk_02000060;
void *Task_Create(u32, u32);
void Task_SetCallbacks(void *, void (*)(void), void (*)(void));
s32 ParticleManager_Initialize(void *, void *);
void Task_Kill(void *);
void ParticleManager_Update(void);
void sub_0801450C(void);

void *sub_080145E4(void *a)
{
    void *p;
    if (gUnk_02000060 != 0)
        return (void *)gUnk_02000060;
    p = Task_Create(10, 0xabc);
    if (p != 0) {
        Task_SetCallbacks(p, ParticleManager_Update, sub_0801450C);
        if (ParticleManager_Initialize(p, a) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
