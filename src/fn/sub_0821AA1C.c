#include "global.h"
extern u8 *gUnk_02000610;
u8 *Script_DecodeOperand(u8 *, s32 *, s32 *);
s32 sub_0821AA1C(u8 *p, s32 *destination)
{
    s32 type, value;
    s32 *vector;
    s32 count;
    vector = destination;
    for (count = 0; count < 3; count++) {
        p = Script_DecodeOperand(p, &type, &value);
        vector[count] = value;
    }
    gUnk_02000610 = p;
    return 0;
}
