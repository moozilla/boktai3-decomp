#include "global.h"
s32 Script_SeekToKeyword(s32);
s32 Script_GetValue(void);
void sub_0822B248(s32);
void sub_0822B280(void)
{
    if (Script_SeekToKeyword(0x74))
        sub_0822B248(Script_GetValue());
}
