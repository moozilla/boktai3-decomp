#include "global.h"
void sub_08020CD4(u8 *, u32, u32);
void sub_0805C73C(u8 *p)
{
    sub_08020CD4(p + 0x17b0, *(u16 *)(p + 0x18), 9);
}
