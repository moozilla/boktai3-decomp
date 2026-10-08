#include "global.h"
extern u32 gUnk_02000488;
void sub_08072C40(u8 *);
void sub_08225938(u8 *);
void sub_08042558(u8 *);
void sub_0821FE6C(u8 *);
void sub_08048898(u8 *);
void sub_08048908(u8 *);
u32 sub_0804D4B0(u8 *p)
{
    if (p[0x254] != 0)
        sub_08072C40(p + 0x244);
    sub_08225938(p + 0x44);
    sub_08042558(p + 0x8C);
    sub_0821FE6C(p + 0x130);
    sub_08048898(p);
    sub_08048908(p);
    return gUnk_02000488 = 0;
}
