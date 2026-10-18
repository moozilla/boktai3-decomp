#include "global.h"

s32 Script_GetValue(void);
u32 MessageWindow_SetNumberArgument(s32, s32);
u32 MessageWindow_SetNumberArgumentFromScript(void)
{
    s32 a = Script_GetValue();
    return MessageWindow_SetNumberArgument(a, Script_GetValue());
}
