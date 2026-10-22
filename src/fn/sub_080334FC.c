#include "global.h"
s32 Script_GetValue(void);
u32 sub_08227E9C(void);
s32 ScrollingDialogue_SetStringArgument(s32, u32);

s32 sub_080334FC(void)
{
    s32 i = Script_GetValue();
    return ScrollingDialogue_SetStringArgument(i, sub_08227E9C());
}
