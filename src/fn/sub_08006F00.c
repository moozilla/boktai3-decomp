#include "global.h"
// MATCHING_CONCESSION: register-pinning
struct Position {
    s16 x, y, z;
};
struct Trigger {
    s16 id, kind;
    s8 actor;
    u8 type;
    u8 pad06[38];
    struct Position *position;
    u8 pad30[8];
    u32 arguments[3];
};
struct Args {
    u32 count : 16;
    u32 unused : 16;
    s32 *values;
};
extern u32 gUnk_02000560[];
s32 sub_0821AD08(u32, struct Args *);
#define Trigger_RunScript sub_08006F00
s32 Trigger_RunScript(struct Trigger *input, u32 scriptInput, s32 eventInput)
{
    struct Trigger *p = ({
        struct Trigger *a = input;
        a;
    });
    u16 script;
    s32 event;
    s32 values[10];
    struct Args args;
    s32 actor, i;
    s32 savedActor;
    u32 *source;
    s32 *out;
    register struct Args *argsPtr asm("r2");
    {
        register s32 arg asm("r2") = eventInput;
        event = arg;
    }
    script = scriptInput;
    if (!script)
        goto fail;
    {
        s32 a = p->actor;
        if (a == -1)
            goto fail;
        asm("" : "+r"(a));
        actor = a;
    }
    {
        register s32 index asm("r2") = actor;
        register u32 mask asm("r3") = 12;
        u32 flags;
        flags = ({
                    register u32 *table asm("r1") = gUnk_02000560;
                    table[index];
                }) &
                mask;
        savedActor = index;
        if (flags)
            goto fail;
    }
    goto active;
fail:
    return -1;
active:
    if (p->type == 1) {
        values[0] = p->position->x;
        values[1] = p->position->y + 64;
        values[2] = p->position->z;
    } else {
        values[0] = p->position->x;
        values[1] = p->position->y;
        values[2] = p->position->z;
    }
    values[3] = p->id;
    values[4] = p->kind;
    argsPtr = &args;
    asm("" : : "r"(argsPtr));
    source = p->arguments;
    out = values + 5;
    i = 2;
    do {
        *out++ = *source++;
    } while (--i >= 0);
    values[8] = savedActor;
    values[9] = event;
    {
        u32 mask = 0xffff0000;
        {
            u32 word = *(u32 *)&args & mask;
            word |= 10;
            *(u32 *)&args = word;
        }
    }
    args.values = values;
    argsPtr = &args;
    return sub_0821AD08(script, argsPtr);
}
