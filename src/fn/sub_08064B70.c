#include "global.h"
void sub_08064B70(u32 a, s32 *x, s32 *y, s32 d)
{
    s32 v;
    switch (a) {
    case 0:
        *x = -4;
        *y = -0xf;
        break;
    case 1:
    case 4:
    case 5:
        *x = -0x10;
        *y = -8;
        break;
    case 2:
        v = -4;
        goto st;
    case 3:
        switch (d) {
        case 0:
            v = -8;
            goto st2;
        case 1:
            v = 0;
        st:
            *x = v;
            *y = v;
            break;
        case 2:
            v = 0;
            goto st2;
        case 3:
            v = -6;
        st2:
            *x = v;
            *y = d;
            break;
        }
        break;
    }
}
