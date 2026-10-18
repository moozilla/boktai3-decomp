#include "global.h"
void Palette_UpdateHighlightPulse(u8 *, u8 *);
void PauseMenu_UpdateHighlightPalette(u8 *p)
{
    Palette_UpdateHighlightPulse(p + 0x43CC, p + 0x43D4);
}
