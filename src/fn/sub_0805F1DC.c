#include "global.h"

extern u8 *gUnk_030042E4;
void sub_08217DB4(u8 *, u8 *);
void sub_0805F324(u8 *, u8 *);

void sub_0805F1DC(u8 *a, u8 *b)
{
    sub_08217DB4(b, gUnk_030042E4 + 0x7540);
    b[0x20] = 0;
    sub_0805F324(a, b);
}
