#include "global.h"
extern u8 *gUnk_02000114;
void Sprite_Remove(u8 *);
void Collider_Remove(u8 *);
void sub_0821D6D0(u8 *);
u32 sub_0805173C(u8 *p)
{
    Sprite_Remove(p + 0x18);
    Collider_Remove(p + 0x60);
    sub_0821D6D0(p + 0xB4);
}
