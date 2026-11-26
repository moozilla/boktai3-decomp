#include "global.h"

struct SwordTask {
    u8 unknown0[0x86];
    u16 value86;
    u8 unknown88[0x1C];
    u16 valueA4;
    u8 unknownA6[0xCAF];
    u32 kind : 8;
    u16 palette, animation;
    u8 unknownD5A[0x12];
    s32 valueD6C, arguments[2];
    u8 unknownD78[8];
    s32 ticks;
};
s32 Script_GetKeywordValue(u32, s32);
s32 Script_GetPc(void);
s32 Script_GetValue(void);
s32 Script_SeekToKeyword(u32);
void Sword_InitAttackComponents(struct SwordTask *);
void Sword_InitTask(struct SwordTask *);
static inline u8 Sword_ClampKind(s32 kind)
{
    s32 value = kind;
    if (kind < 0)
        value = 0;
    else if (kind > 1)
        value = 1;
    return value;
}

s32 Sword_InitializeFromScript(struct SwordTask *p)
{
    s32 i;
    s32 *argument;
    p->kind = Script_GetKeywordValue('t', 0);
    p->kind = Sword_ClampKind(p->kind);
    p->palette = 701;
    p->animation = 402;
    Script_SeekToKeyword('a');
    i = 0;
    argument = p->arguments;
    for (; (i < 2) && Script_GetPc(); i++)
        *(argument++) = Script_GetValue();

    if (Script_SeekToKeyword('e'))
        p->valueD6C = Script_GetValue();
    if (Script_SeekToKeyword('X')) {
        p->valueA4 = (Script_GetPc()) ? (Script_GetValue()) : (0);
        p->value86 = (Script_GetPc()) ? (Script_GetValue()) : (0);
    }
    Sword_InitAttackComponents(p);
    Sword_InitTask(p);
    return p->ticks = 0;
}
