#include "global.h"
#include "ram.h"
u8 *Task_Create(u32, u32);
void Task_SetCallbacks(u8 *, void (*)(void), void (*)(void));
s32 LinkedWidgetSet_Init(u8 *);
void Task_Kill(u8 *);
void FloatingEffectManager_Update(void);
void FloatingEffectManager_Destroy(void);
u8 *FloatingEffectManager_Create(void)
{
    u8 *r;
    if (gLinkedWidgetSet != 0)
        return gLinkedWidgetSet;
    r = Task_Create(9, 0x45c);
    if (r) {
        Task_SetCallbacks(r, FloatingEffectManager_Update, FloatingEffectManager_Destroy);
        if (LinkedWidgetSet_Init(r) < 0) {
            Task_Kill(r);
            return 0;
        }
    }
    return r;
}
