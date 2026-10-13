#include "global.h"
struct A {
    u8 f[0x54];
};
void Anim_Remove(void *);
void Collider_Remove(void *);
void sub_0815F738(void *);
void sub_08194480(u8 *p)
{
    s32 i = 0;
    u8 *a = p + 0x6F60;
    u8 *b = p + 0x543C;
    do {
        Anim_Remove(b);
        Collider_Remove(p + (i * 0x54 + 0x66D8));
        sub_0815F738(a);
        a += 0x10;
        b += 0x60;
        i++;
    } while (i <= 0x19);
}
