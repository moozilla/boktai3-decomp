#include "sprite_trail.h"
void *Task_Create(s32, s32);
void Task_SetCallbacks(void *, void *, void *);
void Task_Kill(void *);
void SpriteGroup_UpdateSlots(void);
void SingletonSlot7_DestroyEffectPool(void);
s32 SingletonSlot7_InitEffectPool(void *);

void *SingletonSlot7_CreateEffectPool(void)
{
    void *r = Task_Create(8, sizeof(struct SpriteTrailPool));
    if (r != 0) {
        Task_SetCallbacks(r, SpriteGroup_UpdateSlots, SingletonSlot7_DestroyEffectPool);
        if (SingletonSlot7_InitEffectPool(r) < 0) {
            Task_Kill(r);
            return 0;
        }
    }
    return r;
}
