#include "global.h"
s32 Script_SeekToKeyword(s32);
s32 Script_GetValue(void);
void sub_08051158(s32, s32, s32, s32, s32);
void sub_08051188(void)
{
    s32 a, b, c, d, e;
    if (Script_SeekToKeyword(0x64)) a = Script_GetValue();
    else a = 0;
    if (Script_SeekToKeyword(0x70)) {
        b = Script_GetValue();
        c = Script_GetValue();
    } else {
        b = 3;
        c = 3;
    }
    if (Script_SeekToKeyword(0x74)) {
        d = Script_GetValue();
        e = Script_GetValue();
    } else {
        d = 0x78;
        e = 0x78;
    }
    sub_08051158(a, b, c, d, e);
}
