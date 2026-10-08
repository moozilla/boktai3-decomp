#include "global.h"
s32 Script_SeekToKeyword(s32);
s32 Script_GetValue(void);
void sub_08056C6C(s32);
void sub_08056CA4(void)
{
    s32 v;
    if (Script_SeekToKeyword(0x70))
        v = Script_GetValue();
    else
        v = 0;
    sub_08056C6C(v);
}
