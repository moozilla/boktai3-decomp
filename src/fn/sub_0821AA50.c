#include "global.h"
extern u8 *gUnk_02000610;
u8 *Script_DecodeOperand(u8 *, s32 *, s32 *);
s32 sub_0821AA50(u8 *p, s16 *destination)
{
    s32 type, value;
    s16 *vector;
    s32 count;
    vector = destination;
    for (count = 0; count < 3; count++) {
        p = Script_DecodeOperand(p, &type, &value);
        vector[count] = value;
    }
    gUnk_02000610 = p;
    return 0;
}
