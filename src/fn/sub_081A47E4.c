#include "global.h"
#include "ram.h"
void *Task_Create(s32, s32);
void Task_SetCallbacks(void *, void *, void *);
void Task_Kill(void *);
void Hresvelgr_UpdateEffects(void);
void Hresvelgr_DestroyEffectPool(void);
s32 EffectSpritePool_Initialize(void *);
void *Hresvelgr_CreateEffectPool(void)
{
    void *p;
    if (gMotionEntryManager == 0) {
        gMotionEntryManager = Task_Create(8, 0xBDC);
        p = gMotionEntryManager;
        if (p != 0) {
            Task_SetCallbacks(p, Hresvelgr_UpdateEffects, Hresvelgr_DestroyEffectPool);
            if (EffectSpritePool_Initialize(p) < 0) {
                Task_Kill(p);
                return 0;
            }
        }
        return p;
    }
    return gMotionEntryManager;
}
