#include "global.h"
void Sprite_Remove(void *);
void sub_082368C4(u8 *s)
{
    Sprite_Remove(s + 0x20);
    Sprite_Remove(s + 0x68);
}
