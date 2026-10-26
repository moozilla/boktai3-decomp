#include "global.h"
#include "ram.h"

u8 *Task_Create(u32, u32);
void Task_SetCallbacks(u8 *, void (*)(void), void (*)(void));
s32 FragmentEffect_InitManager(u8 *);
void Task_Kill(u8 *);
void FragmentEffect_UpdateManager(void);
void FragmentEffect_DestroyManager(void);

u8 *FragmentEffect_GetOrCreateManager(void)
{
    u8 *p;
    if (gFragmentEffectManager != 0)
        return gFragmentEffectManager;
    p = Task_Create(0xa, 0x63c);
    if (p != 0) {
        Task_SetCallbacks(p, FragmentEffect_UpdateManager, FragmentEffect_DestroyManager);
        if (FragmentEffect_InitManager(p) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
