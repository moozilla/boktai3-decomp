#include "global.h"
struct Args { u32 count:16; u32 reserved:16; s32 *values; };
s32 Script_GetValue(void);
u8 *Script_GetPc(void);
u8 *Script_DecodeOperand(u8 *, s32 *, s32 *);
s32 sub_0821AD08(s32, struct Args *);
s32 sub_0821B8AC(void)
{
    s32 values[16], type, value;
    struct Args args;
    s32 id = Script_GetValue();
    u8 *p = Script_GetPc();
    s32 count = 0;
    while (p) {
        p = Script_DecodeOperand(p, &type, &value);
        if (!type)
            break;
        values[count++] = value;
    }
    args.count = count;
    args.values = values;
    return sub_0821AD08(id, &args);
}
