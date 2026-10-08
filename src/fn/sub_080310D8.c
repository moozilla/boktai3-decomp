#include "global.h"
s32 Script_GetValue(void);
s32 Script_GetValueSafe(void);
s32 sub_08031018(s32, s32);
s32 sub_080310D8(void)
{
    s32 a = Script_GetValue();
    return sub_08031018(a, Script_GetValueSafe());
}
