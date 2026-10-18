#include "global.h"

void ModifierMenu_ReleaseTextResources(u8 *);
void Bg_ClearTilemap(u32);
void ModifierMenu_HideDisplayObjects(u8 *);

void ModifierMenu_ClearDisplay(u8 *p)
{
    ModifierMenu_ReleaseTextResources(p);
    Bg_ClearTilemap(0);
    Bg_ClearTilemap(1);
    Bg_ClearTilemap(2);
    Bg_ClearTilemap(3);
    ModifierMenu_HideDisplayObjects(p);
}
