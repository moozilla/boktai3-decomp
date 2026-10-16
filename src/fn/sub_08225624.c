#include "global.h"
#include "engine.h"
struct Trap {
    u32 serial;
    u16 event, other, id, t, flags, count; /* event and other: '?' and '*' wildcards */
    u16 w[4], names[4];
    u32 b, command;
    void *region;
};
u32 sub_0821DDBC(void);
s32 sub_0821AA00(const void *);
u8 *Script_DecodeOperand(u8 *p, s32 *type, s32 *value);
void *sub_0821E104(u16 id, u16 *count);
void sub_0821E190(struct Trap *trap, u8 *top);
/* Script command `trap`: id and a second value (both positional; '?' is a
   wildcard), -m event, -t, -w and -s
   name lists, -b, and -e block or -p procedure run when it fires; registered
   with sub_0821E190 for the map regions named id. */
s32 Trap_CreateFromScript(u8 *top)
{
    struct Trap trap;
    s32 result;
    s32 i;
    u8 *pc;
    u16 *list, *names, *count;
    s32 type, value;
    sub_0821DDBC();
    Memory_Clear(&trap, sizeof(trap));
    trap.id = Script_GetValue();
    trap.other = Script_GetValue();
    if (Script_SeekToKeyword('m'))
        trap.event = Script_GetValue();
    else
        trap.event = 0xDD2;
    trap.t = Script_GetKeywordValue('t', 0);
    trap.flags = 0;
    if (trap.other == '?')
        trap.other = 0x14C9;
    if (trap.event == '?')
        trap.event = 0x14C9;
    else if (trap.event == '*')
        trap.event = 0x1516;
    list = trap.w;
    names = trap.names;
    count = &trap.count;
    {
        u16 *base = list;
        s32 zero = 0;
        u16 *out = trap.w + 3;
        do {
            *out-- = zero;
        } while ((s32)out >= (s32)base);
    }
    if (Script_SeekToKeyword('w'))
        for (i = 0; i < 4 && (pc = Script_GetPc()); i++)
            *list++ = sub_0821AA00(pc);
    {
        u16 *base = *&names; /* taking the address keeps names+3 from being hoisted */
        s32 zero = 0;
        u16 *out = names + 3;
        do {
            *out-- = zero;
        } while ((s32)out >= (s32)base);
    }
    if (Script_SeekToKeyword('s'))
        for (i = 0, list = names; i < 4 && (pc = Script_GetPc()); i++)
            *list++ = sub_0821AA00(pc);
    if (Script_SeekToKeyword('b')) {
        trap.flags |= 0x10;
        trap.b = Script_GetValue();
    }
    if (Script_SeekToKeyword('e')) {
        Script_DecodeOperand(Script_GetPc(), &type, &value);
        trap.command = value;
    } else if (Script_SeekToKeyword('p')) {
        trap.flags |= 0x20;
        trap.command = Script_GetValue();
    }
    trap.flags |= 0; /* reproduces the original's reload and store */
    trap.count = 0;
    trap.region = sub_0821E104(trap.id, count);
    if (trap.region) {
        sub_0821E190(&trap, top);
        result = 0;
    } else
        result = -1;
    return result;
}
