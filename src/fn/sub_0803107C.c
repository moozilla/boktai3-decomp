#include "global.h"
s32 Script_GetValue(void);
s32 sub_08227E9C(void);
s32 sub_08031018(s32, s32);
s32 sub_0803107C(void)
{
    s32 a = Script_GetValue();
    return sub_08031018(a, sub_08227E9C());
}
