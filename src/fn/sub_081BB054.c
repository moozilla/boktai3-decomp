#include "global.h"

void Anim_Remove(void *);

void MenuCursor_OnDestroy(u8 *p)
{
    Anim_Remove(p + 0x18);
}
