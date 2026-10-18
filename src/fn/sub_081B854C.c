#include "global.h"
void Sprite_Remove(u8 *);
void Collider_Remove(u8 *);
void Shadow_Destroy(u8 *);
void DungeonProp_Destroy(u8 *p)
{
    Sprite_Remove(p + 0x18);
    Collider_Remove(p + 0x94);
    if (*(s16 *)(p + 0xec) == 0xc)
        Shadow_Destroy(p + 0xfc);
}
