#include "global.h"
s32 Script_SeekToKeyword(s32);
void Script_GetValue(void);
void Script_ConsumeOptionalKeywordValue(void)
{
    if (Script_SeekToKeyword(0x6b))
        Script_GetValue();
}
