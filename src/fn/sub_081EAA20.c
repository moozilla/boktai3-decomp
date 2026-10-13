#include "global.h"
/* Release the nine animated display objects. */
void Anim_Remove(void *);
void sub_081EAA20(u8 *p)
{
    Anim_Remove(p + 0x3d0);
    Anim_Remove(p + 0x348);
    Anim_Remove(p + 0x458);
    Anim_Remove(p + 0x1b0);
    Anim_Remove(p + 0x128);
    Anim_Remove(p + 0x2c0);
    Anim_Remove(p + 0x238);
    Anim_Remove(p + 0xa0);
    Anim_Remove(p + 0x18);
}
