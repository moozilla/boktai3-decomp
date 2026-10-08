#include "global.h"

void sub_08214514(u8 *);
void sub_082195E0(u8 *);
void sub_0805E468(u8 *);
void sub_0805E2F0(u8 *);
void sub_08030BF8(void);
void sub_08033468(void);

s32 sub_080632EC(u8 *p)
{
    u8 *q;
    s32 i;
    sub_08214514(p + 0x60);
    q = p + 0xa8;
    i = 0x44;
    do {
        sub_082195E0(q);
        q += 0x60;
        i--;
    } while (i >= 0);
    sub_0805E468(p + 0x1af0);
    sub_0805E2F0(p + 0x1c14);
    sub_08030BF8();
    sub_08033468();
    return 0;
}
