#include "global.h"
#include "ram.h"

u8 *Task_Create(u32, u32);
void Task_SetCallbacks(u8 *, void (*)(void), void (*)(void));
s32 sub_0810E6E4(u8 *);
void Task_Kill(u8 *);
void SceneTask_UpdateConditionalCallback(void);
void Registry3C9F_OnDestroy(void);

u8 *Registry3C9F_Create(void)
{
    u8 *p;
    if (gConfirmPanel != 0)
        return gConfirmPanel;
    p = Task_Create(0xb, 0xd38);
    if (p != 0) {
        Task_SetCallbacks(p, SceneTask_UpdateConditionalCallback, Registry3C9F_OnDestroy);
        if (sub_0810E6E4(p) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
