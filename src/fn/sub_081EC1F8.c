#include "global.h"

void Anim_Remove(void *);

void Results_Destroy(u8 *p)
{
    Anim_Remove(p + 0x18);
}
