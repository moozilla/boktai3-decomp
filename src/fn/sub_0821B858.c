#include "global.h"
u8 *Script_DecodeOperand(u8 *, s32 *, s32 *);
void Script_ConvertTextToShiftJis(u8 *, u8 *);
s32 Script_ExecutePrint(u8 *p)
{
    u8 buffer[512];
    s32 type, value;
    while (p) {
        p = Script_DecodeOperand(p, &type, &value);
        if (!type)
            break;
        if (type == 7)
            Script_ConvertTextToShiftJis(buffer, (u8 *)value);
        else if (type == 14) {
            u8 *q = (u8 *)value;
            while (*q)
                q++;
        }
    }
    return 0;
}
