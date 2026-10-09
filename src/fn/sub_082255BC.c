#include "global.h"
struct Message { u32 id:16; u32 reserved:8; u32 count:8; s16 *values; };
u32 Script_GetValue(void);
u8 *Script_GetPc(void);
void sub_0821A340(struct Message *);
s32 sub_082255BC(void)
{
    s16 values[16];
    struct Message message;
    s16 *p;
    u16 count;
    message.id = Script_GetValue();
    message.values = values;
    p = values;
    count = 0;
    while (Script_GetPc()) {
        *p++ = Script_GetValue();
        count = ((u32)count << 16) + 0x10000u >> 16;
    }
    message.count = count;
    sub_0821A340(&message);
    return 0;
}
