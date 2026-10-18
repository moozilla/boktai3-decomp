#include "global.h"

s32 Script_GetValue(void);
u32 MessageWindow_SetPageAdvanceDelay(s32);
u32 MessageWindow_SetPageAdvanceDelayFromScript(void)
{
    return MessageWindow_SetPageAdvanceDelay(Script_GetValue());
}
