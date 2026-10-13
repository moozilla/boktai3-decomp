#include "global.h"
void Anim_Remove(u8 *);
void sub_081EBCF4(u8 *p)
{
    Anim_Remove(p + 0xa8);
    Anim_Remove(p + 0x48);
}
