#include "global.h"
void Anim_Remove(void *);
void Registry1909_Destroy(u8 *p)
{
    Anim_Remove(p + 0x18);
}
