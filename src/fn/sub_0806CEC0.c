#include "global.h"

extern u8 *gUnk_02000188;
u8 *Task_Create(u32, u32);
void Task_SetCallbacks(u8 *, void (*)(void), void (*)(void));
s32 sub_0806CE98(u8 *);
void Task_Kill(u8 *);
void sub_0806CDC0(void);
void sub_0806CE24(void);

u8 *sub_0806CEC0(void)
{
    u8 *p;
    if (gUnk_02000188 != 0)
        return gUnk_02000188;
    p = Task_Create(10, 0x97C);
    if (p != 0) {
        Task_SetCallbacks(p, sub_0806CDC0, sub_0806CE24);
        if (sub_0806CE98(p) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
