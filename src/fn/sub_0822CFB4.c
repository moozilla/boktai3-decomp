#include "global.h"
s32 Script_SeekToKeyword(s32);
s32 Script_GetValue(void);
s32 sub_0822CF94(s32);
s32 sub_0822CFB4(void)
{
    if (Script_SeekToKeyword(0x69))
        return sub_0822CF94(Script_GetValue());
    return 0;
}
