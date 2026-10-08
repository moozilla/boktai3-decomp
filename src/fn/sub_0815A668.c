#include "global.h"
s32 Script_SeekToKeyword(s32);
s32 Script_GetValue(void);
void sub_08159164(s32);
void sub_0815A668(void)
{
    if (Script_SeekToKeyword(0x65))
        sub_08159164(Script_GetValue());
}
