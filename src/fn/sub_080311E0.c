#include "global.h"
u8 *Script_GetValue(void);
u32 MessageWindow_SetPalette(u8 *);
u32 MessageWindow_SetPaletteFromScript(void)
{
    return MessageWindow_SetPalette(Script_GetValue());
}
