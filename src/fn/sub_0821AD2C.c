#include "global.h"
struct Args { u32 count:16; u32 reserved:16; s32 *values; };
u8 *Script_DecodeOperand(u8 *, s32 *, s32 *);
s32 sub_0821AD08(s32, struct Args *);
s32 sub_0821AD2C(u8 *p)
{
    s32 values[16], type, value;
    struct Args args;
    s32 high = p[1] << 8;
    s32 id = (s16)(p[0] | high);
    s32 count;
    p += 2;
    count = 0;
    for (;;) {
        p = Script_DecodeOperand(p, &type, &value);
        if (!type)
            break;
        values[count++] = value;
    }
    args.count = count;
    args.values = values;
    return sub_0821AD08(id, &args);
}
