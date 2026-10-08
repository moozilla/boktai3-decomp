#include "global.h"
s32 Script_SeekToKeyword(s32);
s32 Script_GetValue(void);
void sub_0822AF78(s32);
void sub_0822AFD0(void)
{
    if (Script_SeekToKeyword(0x66))
        sub_0822AF78(Script_GetValue());
}
