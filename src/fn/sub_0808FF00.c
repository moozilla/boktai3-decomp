#include "global.h"
s32 Script_GetPc(void);
s32 Script_GetValue(void);
s32 Script_SeekToKeyword(u32);

void Bat_ReadScriptValuePair(u8 *p)
{
    u8 *q = *(u8 **)(p + 0x3D0);
    s32 a = Script_SeekToKeyword('s');
    s32 b;
    if (a) {
        if (Script_GetPc())
            a = Script_GetValue();
        else
            a = 0;
        *(u16 *)(q + 0x404) = a;
        if (Script_GetPc())
            b = Script_GetValue();
        else
            b = 0;
        *(u16 *)(q + 0x406) = b;
    } else {
        *(u16 *)(q + 0x404) = a;
        *(u16 *)(q + 0x406) = a;
    }
}
