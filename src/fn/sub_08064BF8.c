#include "global.h"
void sub_08064BF8(u32 n, s32 *a, s32 *b)
{
    switch (n) {
    case 0:
        *a = -4;
        *b = -15;
        break;
    case 1:
    case 4:
    case 5:
        *a = -16;
        *b = -8;
        break;
    case 2:
    case 3:
        *a = -4;
        *b = -4;
        break;
    }
}
