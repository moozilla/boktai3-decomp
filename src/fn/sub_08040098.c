#include "global.h"
void sub_08215EB4(s32, s32, s32, s32, s32, s32, s32, s32);
void sub_08215F38(s32, s32, s32, s32, s32, s32, s32);
void sub_08040098(s32 a, s32 b, s32 c, s32 d, u32 k)
{
    s32 df;
    s32 f;
    switch (k) {
    case 1: case 8: case 10:
        df = a - b;
        break;
    case 0: case 2: case 3: case 4: case 5: case 6: case 7: case 9:
    default:
        df = b - a;
        break;
    }
    f = 0x31;
    if (df <= 0) {
        f = 0x11;
        if (df < 0) f = 0x21;
    }
    if (k == 9) goto chk;
    if (k != 10) goto B;
chk:
    if (a < 0) {
        if (b < 0) {
            sub_08215EB4(2, c, d, 3, 1, 0x10, 0, 0);
            return;
        }
    }
B:
    sub_08215F38(2, c, d, b, f, 3, 0);
}
