#include "global.h"
void Sprite_Remove(void *);
void Collider_Remove(void *);
void sub_0823233C(void *);
u32 sub_0805D884(u8 *p)
{
    Sprite_Remove(p + 0x18);
    Collider_Remove(p + 0x68);
    sub_0823233C(p + 0x104);
    return 0;
}
