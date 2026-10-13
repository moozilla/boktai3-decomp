#include "global.h"
void Script_Call(u32, u32);
struct S {
    u8 f[0x40];
    u32 a;
    u32 b;
};
void sub_0811B784(struct S *p)
{
    if (p) {
        Script_Call(p->a, 0);
        p->b = 0;
    }
}
