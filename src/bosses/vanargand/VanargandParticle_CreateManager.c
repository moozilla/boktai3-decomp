#include "global.h"
void *Task_Create(u32, u32);
void Task_SetCallbacks(void *, void *, void *);
s32 sub_081FEFFC(void *);
void Task_Kill(void *);
void VanargandParticle_UpdateManager(void);
void VanargandParticle_DestroyManager(void);

void *VanargandParticle_CreateManager(void)
{
    void *p = Task_Create(8, 0x171c);
    if (p) {
        Task_SetCallbacks(p, VanargandParticle_UpdateManager, VanargandParticle_DestroyManager);
        if (sub_081FEFFC(p) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
