#include "directional_force.h"
s32 Script_SeekToKeyword(s32);
s32 Script_GetValue(void);
void DirectionalForce_ConfigureFixed(s32, s32, s32, s32, s32);
void DirectionalForce_ConfigureFixedFromScript(void)
{
    s32 a, b, c, d, e;
    if (Script_SeekToKeyword(0x64))
        a = Script_GetValue();
    else
        a = 0;
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
    DirectionalForce_ConfigureFixed(a, b, c, d, e);
}
