#include "global.h"
void Anim_Remove(u8 *);
void SelectionMenu_RemoveHighlight(u8 *p)
{
    Anim_Remove(p + 0x5b4);
}
