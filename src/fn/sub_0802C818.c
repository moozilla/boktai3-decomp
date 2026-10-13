#include "global.h"
extern u32 gUnk_020000D8;
void *Task_Create(u32, u32);
void Task_SetCallbacks(void *, void (*)(void), void (*)(void));
s32 sub_0802C7E8(void *);
void Task_Kill(void *);
void sub_0802C738(void);
void sub_0802C7A4(void);

void *sub_0802C818(void)
{
    void *p;
    if (gUnk_020000D8 != 0)
        return (void *)gUnk_020000D8;
    p = Task_Create(8, 0x38);
    if (p != 0) {
        Task_SetCallbacks(p, sub_0802C738, sub_0802C7A4);
        if (sub_0802C7E8(p) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
