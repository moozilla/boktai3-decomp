#include "global.h"
void Anim_Remove(u8 *);
void PauseMenu_RemoveHeaderAnimation(u8 *p)
{
    Anim_Remove(p + 0xA9C);
}
