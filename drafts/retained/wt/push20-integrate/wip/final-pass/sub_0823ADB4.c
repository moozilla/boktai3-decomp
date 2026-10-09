#include "global.h"
extern u8 *gUnk_02000710;
u32 sub_0823AC94(u8 *);
void sub_0823ACCC(u8 *);
void sub_0823ACD0(u8 *);
void sub_0823AD18(u8 *);
void sub_0823AD40(u8 *);
void sub_082383D4(u8 *, s32, s32);
void sub_0823AD64(u8 *);
s32 sub_0823ADB4(u8 *p)
{
    u32 v = sub_0823AC94(p);
    *(u32 *)(p + 0x344) = v;
    *(u32 *)(p + 0x1c) = 1;
    *(u32 *)(gUnk_02000710 + 0x628) = 0;
    *(u32 *)(gUnk_02000710 + 0x62c) = 0;
    *(u16 *)(gUnk_02000710 + 0x610) = 0;
    *(u16 *)(gUnk_02000710 + 0x612) = 0;
    sub_0823ACCC(p);
    sub_0823ACD0(p);
    sub_0823AD18(p);
    sub_0823AD40(p);
    sub_082383D4(p, 0, 0);
    sub_0823AD64(p);
    *(u16 *)(p + 0x534) = 0;
    *(u16 *)(p + 0x536) = 0;
    *(u16 *)(p + 0x538) = 0;
    *(u8 *)(p + 0x549) = *(u32 *)(p + 0x18);
    *(u16 *)(p + 0x54e) = 0;
    *(u32 *)(p + 0x550) = 0;
    return 0;
}
