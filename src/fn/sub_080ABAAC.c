#include "global.h"
extern const u32 gUnk_08605F90[];
void sub_080ABAAC(u8 *p)
{
    *(const u32 **)(p + 0x28c) = gUnk_08605F90;
}
