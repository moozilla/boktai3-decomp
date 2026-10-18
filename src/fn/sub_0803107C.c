#include "global.h"
s32 Script_GetValue(void);
s32 sub_08227E9C(void);
s32 MessageWindow_SetStringArgument(s32, s32);
s32 MessageWindow_SetStringReferenceFromScript(void)
{
    s32 a = Script_GetValue();
    return MessageWindow_SetStringArgument(a, sub_08227E9C());
}
