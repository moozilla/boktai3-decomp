#include "global.h"
void *Task_Create(u32, u32);
void Task_SetCallbacks(void *, void *, void *);
s32 sub_081FEFFC(void *);
void Task_Kill(void *);
void SpriteParticle_UpdateManager(void);
void SpriteParticle_DestroyManager(void);

void *SpriteParticle_CreateManager(void)
{
    void *p = Task_Create(8, 0x171c);
    if (p) {
        Task_SetCallbacks(p, SpriteParticle_UpdateManager, SpriteParticle_DestroyManager);
        if (sub_081FEFFC(p) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
