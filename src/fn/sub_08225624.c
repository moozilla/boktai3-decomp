#include "global.h"
#include "engine.h"
#ifdef NONMATCHING
// fuzzy: 93.5%
// Differs only in the two four-word clearing loops: the original compares a
// pointer against the list base (cmp r0, r2; bge) with no loop counter.
struct Trap {
    u32 serial;
    u16 event, f6, id, fa, flags, count;
    u16 w[4], names[4];
    u32 b, command;
    void *region;
};
u32 sub_0821DDBC(void);
s32 sub_0821AA00(const void *);
u8 *Script_DecodeOperand(u8 *p, s32 *type, s32 *value);
void *sub_0821E104(u16 id, u16 *count);
void sub_0821E190(struct Trap *trap, u8 *top);
static inline void readList(u16 *list, s32 keyword)
{
    s32 i;
    u8 *pc;
    s32 j;
    for (j = 3; j >= 0; j--)
        list[j] = 0;
    if (Script_SeekToKeyword(keyword))
        for (i = 0; i < 4 && (pc = Script_GetPc()); i++)
            list[i] = sub_0821AA00(pc);
}
/* Script command `trap`: id and -? (first two values), -m event, -t, -w and -s
   name lists, -b, and -e block or -p procedure run when it fires; registered
   with sub_0821E190 for the map regions named id. */
s32 Trap_CreateFromScript(u8 *top)
{
    struct Trap trap;
    s32 result;
    s32 type, value;
    sub_0821DDBC();
    Memory_Clear(&trap, sizeof(trap));
    trap.id = Script_GetValue();
    trap.f6 = Script_GetValue();
    if (Script_SeekToKeyword('m'))
        trap.event = Script_GetValue();
    else
        trap.event = 0xDD2;
    trap.fa = Script_GetKeywordValue('t', 0);
    trap.flags = 0;
    if (trap.f6 == '?')
        trap.f6 = 0x14C9;
    if (trap.event == '?')
        trap.event = 0x14C9;
    else if (trap.event == '*')
        trap.event = 0x1516;
    readList(trap.w, 'w');
    readList(trap.names, 's');
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
    trap.flags |= 0;
    trap.count = 0;
    trap.region = sub_0821E104(trap.id, &trap.count);
    if (trap.region) {
        sub_0821E190(&trap, top);
        result = 0;
    } else
        result = -1;
    return result;
}
#else
INCLUDE_ASM("asm/nonmatching", Trap_CreateFromScript);
#endif
