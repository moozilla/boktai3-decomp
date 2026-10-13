#include "global.h"

extern void *gUnk_03005420;
void *Task_Create(u32, u32);
void Task_SetCallbacks(void *, void (*)(void), void (*)(void));
s32 sub_08227478(void *, void *);
void Task_Kill(void *);
void sub_08227378(void);
void sub_0822744C(void);

void *sub_082275D8(void *a)
{
    void *p;
    if (gUnk_03005420 != 0)
        return gUnk_03005420;
    p = Task_Create(7, 0xA4);
    if (p != 0) {
        Task_SetCallbacks(p, sub_08227378, sub_0822744C);
        if (sub_08227478(p, a) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
