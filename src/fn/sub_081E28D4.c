#include "global.h"

void sub_08220F70(u8 *, u8 *);

void sub_081E28D4(u8 *p)
{
    sub_08220F70(p + 0x98, p + 0x18);
    sub_08220F70(p + 0xf8, p + 0x18);
    sub_08220F70(p + 0x1b8, p + 0x18);
    (*(u16 *)(p + 0x21e))++;
}
