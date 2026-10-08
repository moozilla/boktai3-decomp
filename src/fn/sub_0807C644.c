#include "global.h"
void sub_0807C394(u32, u32, u32);
void sub_0807C3D8(u32, u32, u32);
void sub_0807C4B4(u32, u32, u32);
void sub_0807C5C4(u32, u32, u32);
void sub_0807C424(u32, u32, u32);
void sub_0807C580(u32, u32, u32);
void sub_0807C604(u32, u32, u32);
void sub_0807C4F4(u32, u32, u32);
void sub_0807C644(u8 a, u8 b, u32 c, u32 d)
{
    switch (a) {
    case 4:
        sub_0807C394(b, c, d);
        break;
    case 1:
        sub_0807C3D8(b, c, d);
        break;
    case 3:
        sub_0807C4B4(b, c, d);
        break;
    case 10:
        sub_0807C5C4(b, c, d);
        break;
    case 5:
    case 6:
    case 7:
    case 8:
        sub_0807C424(b, c, d);
        break;
    case 0x15:
        sub_0807C580(b, c, d);
        break;
    case 0x17:
        sub_0807C604(b, c, d);
        break;
    case 0x19:
        sub_0807C4F4(b, c, d);
        break;
    }
}
