#include "global.h"
extern u32 gUnk_020000B0;
void *Task_Create(u32, u32);
void Task_SetCallbacks(void *, void (*)(void), void (*)(void));
s32 sub_08019AA8(void *);
void Task_Kill(void *);
void SpritePool_Update(void);
void sub_08019A78(void);

void *sub_08019AE4(void)
{
    void *p = (void *)gUnk_020000B0;
    if (p == 0) {
        p = Task_Create(10, 0xb1c);
        if (p != 0) {
            Task_SetCallbacks(p, SpritePool_Update, sub_08019A78);
            if (sub_08019AA8(p) < 0) {
                Task_Kill(p);
                return 0;
            }
        }
    }
    return p;
}
