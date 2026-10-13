#include "global.h"
extern u32 gUnk_03002604;
void *Task_Create(u32, u32);
void Task_SetCallbacks(void *, void (*)(void), void (*)(void));
s32 sub_0822CC60(void *);
void Task_Kill(void *);
void sub_0822CA28(void);
void sub_0822CA48(void);

void *sub_0822CC80(void)
{
    void *p;
    if (gUnk_03002604 != 0)
        return (void *)gUnk_03002604;
    p = Task_Create(5, 0x30);
    if (p != 0) {
        Task_SetCallbacks(p, sub_0822CA28, sub_0822CA48);
        if (sub_0822CC60(p) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
