#include "global.h"
u32 Script_GetKeywordValue(u32, u32);
extern u32 gUnk_030051E4;
void sub_08227E00(void)
{
    gUnk_030051E4 = Script_GetKeywordValue(0x74, 0);
}
