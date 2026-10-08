#include "global.h"
s32 Script_SeekToKeyword(s32);
s32 Script_GetValue(void);
void sub_0822B1D8(s32);
void sub_0822B230(void)
{
    if (Script_SeekToKeyword(0x66))
        sub_0822B1D8(Script_GetValue());
}
