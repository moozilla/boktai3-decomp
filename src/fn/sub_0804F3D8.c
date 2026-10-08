#include "global.h"
void sub_0804F384(u8 *);
void sub_0804EFB4(u8 *);
void sub_0804F0B4(u8 *);
void sub_0804F140(u8 *);
void sub_0804F1FC(u8 *);
u32 sub_0804F3D8(u8 *p)
{
    sub_0804F384(p);
    switch (p[0x262]) {
    case 0:
        sub_0804EFB4(p);
        break;
    case 1:
        sub_0804F0B4(p);
        break;
    }
    sub_0804F140(p);
    sub_0804F1FC(p);
    return 0;
}
