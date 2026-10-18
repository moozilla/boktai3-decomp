#include "global.h"
u32 Script_SeekToKeyword(u32);
s32 Script_GetValue(void);
s32 Script_GetOptionalIValue(void)
{
    if (Script_SeekToKeyword(0x69) != 0)
        return Script_GetValue();
    return 0;
}
