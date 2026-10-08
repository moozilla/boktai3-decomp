#include "global.h"
void sub_08163E68(u32);
void sub_08020D08(u8 *);
s32 sub_081CE040(u8 *a, u8 *b)
{
    if (b[1] == 0x1d) sub_08163E68(*(u32 *)(b + 0x20));
    sub_08020D08(b + 0x168);
}
