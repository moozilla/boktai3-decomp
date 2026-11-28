#include "global.h"
#include "ui/save/save_menu.h"

void LinkScorePanel_InitGlyphFrames(struct SaveMenu *menu)
{
    s32 i;
    menu->glyphs[0] = 0x3d;
    for (i = 0; i < 18; i++)
        menu->glyphs[i + 1] = 0x3e;
}
