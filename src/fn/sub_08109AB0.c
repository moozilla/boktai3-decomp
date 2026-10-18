#include "global.h"

u8 *Task_Create(u32, u32);
void Task_SetCallbacks(u8 *, void (*)(void), void (*)(void));
s32 MessagePanel_InitFromScript(u8 *);
void Task_Kill(u8 *);
void MessagePanel_Update(void);
void LinkConnectionMenu_OnDestroy(void);

u8 *LinkConnectionMenu_CreateFromScript(void)
{
    u8 *p;
    p = Task_Create(11, 0x4C);
    if (p != 0) {
        Task_SetCallbacks(p, MessagePanel_Update, LinkConnectionMenu_OnDestroy);
        if (MessagePanel_InitFromScript(p) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
