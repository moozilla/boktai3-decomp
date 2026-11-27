#include "monsters/sword/sword_task.h"
#include "global.h"

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
    p->variant = Script_GetKeywordValue('t', 0);
    p->variant = Sword_ClampKind(p->variant);
    p->paletteBank = 701;
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
        p->field86 = (Script_GetPc()) ? (Script_GetValue()) : (0);
    }
    Sword_InitAttackComponents(p);
    Sword_InitTask(p);
    return p->ticks = 0;
}
