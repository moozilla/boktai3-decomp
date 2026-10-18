#include "global.h"
extern void *gUnk_02000240;
void *Task_Create(s32, s32);
void Task_SetCallbacks(void *, void *, void *);
void Task_Kill(void *);
void Hresvelgr_UpdateEffects(void);
void Hresvelgr_DestroyEffectPool(void);
s32 sub_081A4760(void *);
void *Hresvelgr_CreateEffectPool(void)
{
    void *p;
    if (gUnk_02000240 == 0) {
        gUnk_02000240 = Task_Create(8, 0xBDC);
        p = gUnk_02000240;
        if (p != 0) {
            Task_SetCallbacks(p, Hresvelgr_UpdateEffects, Hresvelgr_DestroyEffectPool);
            if (sub_081A4760(p) < 0) {
                Task_Kill(p);
                return 0;
            }
        }
        return p;
    }
    return gUnk_02000240;
}
