#include "global.h"
#define GridAttack_Update sub_0802BF30

void sub_0802B7C8(u8 *p);
void sub_0802B7A4(u8 *p);
void sub_0802AAD4(u8 *p);
void sub_0802BAFC(u8 *p);
void sub_08029F64(u8 *p);
void sub_08029FB0(u8 *p);

int sub_0802BF30(u8 *p)
{
    u16 *t;
    sub_0802B7C8(p);
    sub_0802B7A4(p);
    sub_0802AAD4(p);
    sub_0802BAFC(p);
    if (*(u16 *)(p + 0x1aa) <= 0xef)
        sub_08029F64(p);
    else
        sub_08029FB0(p);
    t = (u16 *)(p + 0x1aa);
    (*t)++;
    if (*t > 0x12b)
        *t = 0;
    return 0;
}
