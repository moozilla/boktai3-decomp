#include "global.h"
void sub_08003AA8(s32, s32, s32 *);
/* sub_08003AA8(n, 7, {3, a..f}) with n = kind + 1 for kinds 1-4, else 1. */
void sub_082279A8(u32 kind, s32 a, s32 b, s32 c, s32 d, s32 e, s32 f)
{
    s32 args[8];
    s32 n;
    switch (kind) {
        case 1:
            n = 2;
            break;
        case 2:
            n = 3;
            break;
        case 3:
            n = 4;
            break;
        case 4:
            n = 5;
            break;
        case 0:
        default:
            n = 1;
            break;
    }
    args[0] = 3;
    args[1] = a;
    args[2] = b;
    args[3] = c;
    args[4] = d;
    args[5] = e;
    args[6] = f;
    sub_08003AA8(n, 7, args);
}
