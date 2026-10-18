#include "global.h"

s32 Sprite_Remove(u8 *);

s32 EnemyD459_Destroy(u8 *p)
{
    return Sprite_Remove(p + 0x24);
}
