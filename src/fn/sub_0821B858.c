#include "global.h"
u8 *Script_DecodeOperand(u8 *, s32 *, s32 *);
void sub_0821B6C8(u8 *, u8 *);
s32 sub_0821B858(u8 *p)
{
    u8 buffer[512];
    s32 type, value;
    while (p) {
        p = Script_DecodeOperand(p, &type, &value);
        if (!type)
            break;
        if (type == 7)
            sub_0821B6C8(buffer, (u8 *)value);
        else if (type == 14) {
            u8 *q = (u8 *)value;
            while (*q)
                q++;
        }
    }
    return 0;
}
