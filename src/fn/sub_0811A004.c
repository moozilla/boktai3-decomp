#include "global.h"
extern u32 gUnk_020004F0[];
u32 Script_GetKeywordValue(u32, u32);
void StageIconDisplay_Create(u32, u32, void (*)(void), u32);
void sub_08119F9C(void);
void sub_0811A004(void)
{
    u32 p;
    u32 i = Script_GetKeywordValue(0x69, 0);
    p = gUnk_020004F0[i];
    if (p != 0)
        StageIconDisplay_Create(0, 0, sub_08119F9C, p);
}
