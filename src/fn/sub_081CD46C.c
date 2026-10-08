#include "global.h"

void sub_08020CD4(u8 *, u32, u32);

void sub_081CD46C(u8 *a, u32 b)
{
    u32 buf[4];
    sub_08020CD4(a + 0x168, b, 5);
}
