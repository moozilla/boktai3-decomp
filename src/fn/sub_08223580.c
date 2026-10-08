#include "global.h"

void sub_0824722C(u32);

struct S { u8 a, b, c, d; };

void sub_08223580(u8 a) {
    if (**(u8 **)0x03006A80 == 0) {
        u8 t = ((struct S *)0x03005390)->c;
        a = 0;
        if (t == 1)
            a = 1;
    } else {
        u8 *p = (u8 *)0x03005390;
        ((volatile u8 *)p)[3];
        p[3] = 0;
    }
    sub_0824722C(a);
}
