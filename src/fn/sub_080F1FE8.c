#include "global.h"

/* Split the RGB555 colour at 0xD32 into its 5-bit channels at 0xCC4..0xCC6.
   The original is declared to return a value but has no return statement:
   r0 stays live to the end, so the argument is copied out of it and the
   epilogue pops into r1. */
u32 sub_080F1FE8(u8 *p)
{
    u16 *color = (u16 *)(p + 0xD32);
    u8 r = *color & 0x1F;
    p[0xCC4] = r;
    r = (*color >> 5) & 0x1F;
    p[0xCC5] = r;
    r = (*color >> 10) & 0x1F;
    p[0xCC6] = r;
}
