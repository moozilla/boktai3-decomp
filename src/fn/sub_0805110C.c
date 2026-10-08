#include "global.h"
s32 Script_SeekToKeyword(s32);
s32 Script_GetValue(void);
void sub_080510E0(s32, s32, s32, s32);
void sub_0805110C(void)
{
    s32 a, b, c, d;
    if (Script_SeekToKeyword(0x70)) {
        a = Script_GetValue();
        b = Script_GetValue();
    } else {
        a = 3;
        b = 3;
    }
    if (Script_SeekToKeyword(0x74)) {
        c = Script_GetValue();
        d = Script_GetValue();
    } else {
        c = 0x78;
        d = 0x78;
    }
    sub_080510E0(a, b, c, d);
}
