#include "global.h"
#include "engine.h"

struct Args08197F68 {
    u16 count;
    s32 *values;
    s32 buf[8];
};
struct Task08197F68 {
    u8 pad0[0x18];
    u32 channel;
    void *script1;
    void *script2;
    u16 keys;
    u16 flags;
    struct Args08197F68 args;
};

s32 sub_08197F68(struct Task08197F68 *t)
{
    s32 i;
    t->script1 = NULL;
    t->script2 = NULL;
    t->flags = 1;
    if (Script_SeekToKeyword(0x70))
        t->script2 = (void *)Script_GetValue();
    if (Script_SeekToKeyword(0x65))
        t->script1 = (void *)Script_GetValue();
    if (Script_SeekToKeyword(0x63))
        t->keys = Script_GetValue();
    if (Script_SeekToKeyword(0x64))
        t->flags &= ~1;
    if (Script_SeekToKeyword(0x50))
        t->flags |= 2;
    t->args.count = 0;
    t->args.values = t->args.buf;
    if (Script_SeekToKeyword(0x61)) {
        for (i = 0; i <= 7 && Script_GetPc(); i++)
            t->args.values[i] = Script_GetValue();
        t->args.count = i;
    }
    return 0;
}
