#include "global.h"

struct A {
    u32 a, b;
};
struct B {
    u32 flags;
    struct A *args;
};

void Script_Call(u32, struct B *);
void Task_Kill(u8 *);

s32 sub_08063248(u8 *p)
{
    if (*(u16 *)(p + 0x1AEE) > 0x1f) {
        u32 v = *(u32 *)(p + 0x1CF4);
        if (v != 0) {
            struct A a;
            struct B b;
            b.flags = (b.flags & 0xFFFF0000) | 2;
            a.a = *(u8 *)(p + 0x1AAC);
            a.b = *(u8 *)(p + 0x1AAD);
            b.args = &a;
            Script_Call(v, &b);
        }
        Task_Kill(p);
    }
    return 0;
}
