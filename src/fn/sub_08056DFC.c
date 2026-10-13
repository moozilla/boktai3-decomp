#include "global.h"

void Anim_Remove(u8 *);

u32 sub_08056DFC(u8 *p)
{
    Anim_Remove(p + 0x38);
    return 0;
}
