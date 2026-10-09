#include "global.h"
u8 *Script_DecodeOperand(u8 *, s32 *, s32 *);
u8 *sub_0821B738(u8 *p)
{
    s32 type, condition, block;
condition_next:
    p = Script_DecodeOperand(p, &type, &condition);
block_next:
    p = Script_DecodeOperand(p, &type, &block);
    if (condition)
        return (u8 *)block;
    p = Script_DecodeOperand(p, &type, &condition);
    if (p) {
        type >>= 16;
        p = (u8 *)condition;
        if (type == 'e') {
            condition = 1;
            goto block_next;
        }
        if (type == 'i')
            goto condition_next;
    }
    return 0;
}
