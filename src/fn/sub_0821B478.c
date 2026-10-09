#include "global.h"
u8 *Script_DecodeOperand(u8 *, s32 *, s32 *);
void sub_08219EBC(u8 *, u8 *, s32);
struct Reference { u32 descriptor; u16 count; u16 index; };
u8 *sub_0821B478(u8 *p, struct Reference *ref)
{
    s32 type, count, index;
    u8 *end;
    sub_08219EBC((u8 *)ref, p, 4);
    if ((*(u8 *)ref & 0xF0) == 0x20) {
        end = Script_DecodeOperand(p + 4, &type, &count);
        end = Script_DecodeOperand(end, &type, &index);
        ref->count = count;
        ref->index = index;
    } else {
        end = p + 4;
        ref->count = 1;
        ref->index = 0;
    }
    return end;
}
