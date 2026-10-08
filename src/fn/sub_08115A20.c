#include "global.h"
extern u32 gUnk_030053F4, gUnk_030053F0;
struct S { u8 f[0xee]; u8 st; };
struct T { u32 a; u32 b; };
void sub_0811BC28(u32);
void sub_08115A20(struct S *s, struct T *t)
{
    u32 m = 0x800;
    if (((gUnk_030053F4 | gUnk_030053F0) & m) == 0 && t->b == 1) {
        switch (s->st) {
        case 0:
            sub_0811BC28(1);
            break;
        case 1:
            sub_0811BC28(2);
            break;
        }
    }
}
