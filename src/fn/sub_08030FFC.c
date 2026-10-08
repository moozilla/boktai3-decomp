#include "global.h"

s32 Script_GetValue(void);
u32 sub_08030FD4(s32, s32);
u32 sub_08030FFC(void)
{
    s32 a = Script_GetValue();
    return sub_08030FD4(a, Script_GetValue());
}
