#include "global.h"

extern u8 *gUnk_020001B8;
u8 *Task_Create(u32, u32);
void Task_SetCallbacks(u8 *, void (*)(void), void (*)(void));
s32 sub_0810845C(u8 *);
void Task_Kill(u8 *);
void sub_08108360(void);
void sub_081083F4(void);

u8 *sub_08108490(void)
{
    u8 *p;
    if (gUnk_020001B8 != 0)
        return gUnk_020001B8;
    p = Task_Create(0x8, 0x6c);
    if (p != 0) {
        Task_SetCallbacks(p, sub_08108360, sub_081083F4);
        if (sub_0810845C(p) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
