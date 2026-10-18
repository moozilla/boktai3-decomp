#include "global.h"
u8 *Script_GetValue(void);
u32 MessageWindow_SetDisplayMode(u8 *);
u32 MessageWindow_SetDisplayModeFromScript(void)
{
    return MessageWindow_SetDisplayMode(Script_GetValue());
}
