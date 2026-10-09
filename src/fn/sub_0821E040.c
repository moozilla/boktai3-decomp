#include "global.h"
struct Trigger { u16 id, pad; s16 *position; };
struct Trap { u32 serial; u16 f4, f6, f8, fa, flags, fe; u16 w[4], s[4]; u32 f20, command; void *f28; };
struct Args { u32 count:16; u32 reserved:16; u32 *values; };
void sub_08224F20(u32, struct Args *);
void sub_08224F08(u32, struct Args *);
void sub_0821E040(struct Trigger *trigger, struct Trap *trap, u16 event)
{
    u32 values[14];
    struct Args args;
    u32 *out = values;
    s32 i;
    u32 command;
    *out++ = trigger->id;
    *out++ = trap->f8;
    *out++ = event;
    *out++ = trigger->position[0];
    *out++ = trigger->position[1];
    *out++ = trigger->position[2];
    for (i = 0; i < 4; i++) *out++ = trap->w[i];
    command = trap->command;
    for (i = 0; i < 4; i++) *out++ = trap->s[i];
    args.count = 14;
    args.values = values;
    if (trap->flags & 0x20) sub_08224F20(command, &args);
    else sub_08224F08(command, &args);
}
