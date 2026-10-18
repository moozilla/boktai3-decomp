#include "global.h"
s32 Script_GetValue(void);
s32 Script_GetValueSafe(void);
s32 MessageWindow_SetStringArgument(s32, s32);
s32 MessageWindow_SetArgumentFromScript(void)
{
    s32 a = Script_GetValue();
    return MessageWindow_SetStringArgument(a, Script_GetValueSafe());
}
