#include "global.h"

s32 Div(s32, s32);

s32 sub_0803F934(s32 a, s32 b, s32 c)
{
    switch (b) {
    case 0:
    case 7:
    case 8:
    case 9:
    case 10:
    case 12:
        if (c != 0)
            a = Div(a * c, 10);
        break;
    default:
        a = a + c;
        break;
    }
    return a;
}
