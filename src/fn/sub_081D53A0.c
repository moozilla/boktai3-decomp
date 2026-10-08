#include "global.h"
s32 Script_SeekToKeyword(s32);
void Script_GetValue(void);
void sub_081D53A0(void)
{
    if (Script_SeekToKeyword(0x6b)) Script_GetValue();
}
