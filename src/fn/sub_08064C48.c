#include "global.h"
void sub_080648B0(u32, u32, u32, u32);
void sub_08064918(u32, u32, u32, u32);
void sub_08064954(u32, u32, u32, u32);
void sub_08064990(u32, u32, u32, u32);
void sub_08064A58(u32, u32, u32, u32);
void sub_08064B70(u32, u32, u32, u32);
void sub_08064BF8(u32, u32, u32, u32);
void sub_08064C48(u32 a, u32 b, u32 c, u32 d, u32 e)
{
    c = (s32)((c + 0x20) & 0xFF) >> 6;
    switch (b) {
    case 4:
        sub_080648B0(a, d, e, c);
        break;
    case 1:
        sub_08064918(a, d, e, c);
        break;
    case 3:
        sub_08064954(a, d, e, c);
        break;
    case 5:
    case 6:
    case 7:
    case 8:
        sub_08064990(a, d, e, c);
        break;
    case 21:
        sub_08064A58(a, d, e, c);
        break;
    case 25:
        sub_08064B70(a, d, e, c);
        break;
    default:
        sub_08064BF8(a, d, e, c);
        break;
    }
}
