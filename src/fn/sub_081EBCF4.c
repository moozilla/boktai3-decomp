#include "global.h"
void Anim_Remove(u8 *);
void ScreenParams3F8_DestroyDisplay(u8 *p)
{
    Anim_Remove(p + 0xa8);
    Anim_Remove(p + 0x48);
}
