#include "global.h"

#ifdef NONMATCHING
// fuzzy: 87.5%
// foundry draft (worker best 87.1%); what differs is mostly register allocation and instruction order
#define ModifierStats_ApplyKinds4And14 ModifierStats_ApplyKinds4And14

struct StatModifier {
    s32 (*apply)(struct StatModifier *, s32);
    u32 unknown4 : 8;
    u32 kind : 8;
    u32 unknown6 : 16;
    u8 unknown8[12];
};
struct ModifierStats {
    u8 unknown0[0x54];
    u8 modifierCount;
    u8 unknown55[7];
    s32 value5C, value60, value64;
    u8 unknown68[0x68];
    struct StatModifier modifiers[32];
    u8 unknown350[0x1CC];
    u16 baseValue51C;
};
s32 sub_08249240(void *, s32, s32 (*)(struct StatModifier *, s32));
static inline struct StatModifier *ModifierStats_GetEntry(struct ModifierStats *stats, s32 i)
{
    return &stats->modifiers[i];
}
s32 ModifierStats_ApplyKinds4And14(struct ModifierStats *stats, s32 value)
{
    struct StatModifier *modifier = stats->modifiers;
    s32 i = 0;
    if (i < stats->modifierCount) {
        do {
            struct StatModifier *entry = ModifierStats_GetEntry(stats, i);
            if (entry->kind == 4 || entry->kind == 14)
                value = sub_08249240(modifier, value, entry->apply);
            modifier++;
            i++;
        } while (i < stats->modifierCount);
    }
    return value;
}
#else
INCLUDE_ASM("asm/nonmatching", ModifierStats_ApplyKinds4And14);
#endif
