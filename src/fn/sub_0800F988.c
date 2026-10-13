#include "global.h"

u8 *Task_Create(u32, u32);
void Task_SetCallbacks(u8 *, void (*)(void), void (*)(void));
s32 sub_0800F924(u8 *);
void Task_Kill(u8 *);
void ResultPrompt_Update(void);
void sub_0800F914(void);

u8 *sub_0800F988(void)
{
    u8 *p;
    p = Task_Create(11, 0x2c);
    if (p != 0) {
        Task_SetCallbacks(p, ResultPrompt_Update, sub_0800F914);
        if (sub_0800F924(p) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
