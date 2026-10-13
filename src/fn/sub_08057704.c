#include "global.h"

void Anim_Remove(u8 *);

u32 sub_08057704(u8 *p)
{
    Anim_Remove(p + 0x1C);
    return 0;
}
