#include "global.h"
void sub_08219DD8(void *, u32);
void sub_08202ED8(void *, u32, u32, u32, u32);
void sub_08202EF4(void *, u32, u32, u32, u32, u32);
void sub_08202F0C(void *, u32, u32, u32);
void sub_08202F14(void *, u32, u32, u32);
void sub_08202F1C(void *, u32, u32, s32);
void sub_08202F2C(void *, u32, u32, u32);
void sub_08202F34(void *, void *);
void sub_08202F24(void *, u32, u32);
u32 sub_08202654(void);
void sub_08202E4C(u8 *p)
{
    u32 z;
    sub_08219DD8(p, 0x5c);
    z = 0;
    sub_08202ED8(p, 0, 0x3c, 0, z);
    sub_08202EF4(p, 0, 0, 0xa, 0x14, 0x14);
    sub_08202F0C(p, 8, 0x20, 8);
    sub_08202F14(p, 0, 0, 0);
    sub_08202F1C(p, 0x848F, 0, -1);
    sub_08202F2C(p, 0, 0, 0);
    sub_08202F34(p, sub_08202654);
    sub_08202F24(p, 0, 0);
    *(u16 *)(p + 0x4e) = z;
    *(u16 *)(p + 0x58) = z;
}
