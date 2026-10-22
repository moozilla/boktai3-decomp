#include "global.h"
s32 Script_GetValue(void);
u32 Script_GetTextSourcePc(void);
s32 ScrollingDialogue_SetStringArgument(s32, u32);

s32 sub_080334E0(void)
{
    s32 i = Script_GetValue();
    return ScrollingDialogue_SetStringArgument(i, Script_GetTextSourcePc());
}
