#include "global.h"

extern u8 *gUnk_020001B0;
u8 *Task_Create(u32, u32);
void Task_SetCallbacks(u8 *, void (*)(void), void (*)(void));
s32 Pool184_InitManager(u8 *);
void Task_Kill(u8 *);
void sub_08104D6C(void);
void CallbackPool_DestroyEntries(void);

u8 *Pool184_CreateManager(void)
{
    u8 *p;
    if (gUnk_020001B0 != 0)
        return gUnk_020001B0;
    p = Task_Create(0xa, 0x630);
    if (p != 0) {
        Task_SetCallbacks(p, sub_08104D6C, CallbackPool_DestroyEntries);
        if (Pool184_InitManager(p) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
