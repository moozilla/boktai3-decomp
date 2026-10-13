#include "global.h"

struct S {
    u8 f[0x18];
    u8 v;
};
extern struct S *gUnk_02000490;
struct S *Task_Create(s32, s32);
void Task_SetCallbacks(struct S *, void (*)(void), void (*)(void));
void sub_080546B8(void);
void sub_08054728(void);

struct S *sub_08054AD0(void)
{
    struct S *p = Task_Create(8, 0x8FC);
    if (p) {
        Task_SetCallbacks(p, sub_080546B8, sub_08054728);
        p->v = 0;
    }
    gUnk_02000490 = p;
    return p;
}
