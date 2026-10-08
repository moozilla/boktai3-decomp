#include "global.h"

struct T { u8 f[0x20]; u16 a; u16 b; };
struct T *sub_08048598(void);
void sub_080485E0(u32 a, u32 b)
{
    u8 *p = (u8 *)sub_08048598();
    if (p) {
        struct T *t = (struct T *)(p + 0x44);
        t->a = a;
        t->b = b;
    }
}
