#include "global.h"
extern u32 gUnk_02000070;
void *Task_Create(u32, u32);
void Task_SetCallbacks(void *, void (*)(void), void (*)(void));
s32 sub_08016634(void *, u16);
void Task_Kill(void *);
void PaletteLevels_Update(void);
void sub_08016620(void);

void *sub_08016674(u32 a)
{
    void *p;
    if (gUnk_02000070 != 0)
        return (void *)gUnk_02000070;
    p = Task_Create(9, 0x34);
    if (p != 0) {
        Task_SetCallbacks(p, PaletteLevels_Update, sub_08016620);
        if (sub_08016634(p, a) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
