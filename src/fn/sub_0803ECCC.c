#include "global.h"

void sub_0811C168(u8 *);
void sub_0811EE0C(u8 *);
void sub_0811C09C(u8 *);
void sub_0822B2F8(u32);
void sub_0803C008(u32, u8 *, u8 *, u32);
s32 Mod(s32, s32);

u32 sub_0803ECCC(u32 a, u8 *b, u8 *e, u32 d)
{
    u32 f;
    u32 z;
    u32 m;
    if (e[0x64] != 0) {
        z = 0;
        e[0x64] = z;
        sub_0811C168(b);
        sub_0811EE0C(b);
        *(u32 *)(b + 0x5ac) = z;
        *(u32 *)(b + 0x5b4) = z;
        *(u32 *)(b + 0xc0) = z;
        *(u32 *)(b + 0xc8) = z;
    }
    m = 2;
    *(u32 *)(b + 0x5fc) |= m;
    *(u32 *)(b + 0x5fc) |= 1;
    *(u32 *)(b + 0x5fc) |= 0xc;
    f = *(u32 *)(e + 0x44);
    if (f <= 0x1d) {
        if (Mod(f, 0xc) == 0) {
            sub_0811C09C(b);
            sub_0822B2F8(0x16e);
        }
    } else if (f <= 0x3b) {
        if (Mod(f, 6) == 0) {
            sub_0811C09C(b);
            sub_0822B2F8(0x16e);
        }
    }
    if (*(u32 *)(e + 0x44) > 0x77)
        sub_0803C008(a, b, e, d);
    (*(u32 *)(e + 0x44))++;
    return 0;
}
