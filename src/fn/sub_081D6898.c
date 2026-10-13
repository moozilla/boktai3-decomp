#include "global.h"

void Anim_Remove(u8 *);

void sub_081D6898(u8 *p)
{
    if (p[4] != 0) {
        Anim_Remove(p);
    }
}
