#include "global.h"
void *Task_Create(s32, s32);
void Task_SetCallbacks(void *, void *, void *);
void Task_Kill(void *);
void SpriteGroup_UpdateSlots(void);
void SingletonSlot7_DestroyEffectPool(void);
s32 SingletonSlot7_InitEffectPool(void *);

void *SingletonSlot7_CreateEffectPool(void)
{
    void *r = Task_Create(8, 0x980);
    if (r != 0) {
        Task_SetCallbacks(r, SpriteGroup_UpdateSlots, SingletonSlot7_DestroyEffectPool);
        if (SingletonSlot7_InitEffectPool(r) < 0) {
            Task_Kill(r);
            return 0;
        }
    }
    return r;
}
