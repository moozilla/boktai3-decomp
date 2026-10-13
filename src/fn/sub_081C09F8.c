#include "global.h"
struct S {
    u8 f[0x30];
    void *a;
    u8 g[0x8];
    void *b;
    u8 h[0x14];
    s32 n;
};
void Script_Call(void *, u32);
void Task_Kill(void *);
void sub_081C09F8(struct S *p)
{
    p->n = p->n + 1;
    if (p->n > 0x3f && p->b != 0) {
        Script_Call(p->b, 0);
        Task_Kill(p->a);
        Task_Kill(p);
    }
}
