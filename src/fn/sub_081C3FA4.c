#include "global.h"

extern u32 gUnk_02000268;
void sub_08214514(u8 *);

void sub_081C3FA4(u8 *p)
{
    sub_08214514(p + 0x34);
    gUnk_02000268 = 0;
}
