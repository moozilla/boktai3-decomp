#include "global.h"

#ifdef NONMATCHING
// fuzzy: 87.5%
// foundry draft (worker best 87.1%); what differs is mostly register allocation and instruction order
#define ModifierStats_ApplyKinds2And5 sub_0811EA34

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
static inline struct StatModifier *ModifierStats_GetModifiers(struct ModifierStats *stats)
{
    return stats->modifiers;
}
s32 sub_0811EA34(struct ModifierStats *stats, s32 value)
{
    struct StatModifier *entry = ModifierStats_GetModifiers(stats);
    s32 i = 0;
    if (i < stats->modifierCount) {
        do {
            if (entry->kind == 2 || entry->kind == 5)
                value = sub_08249240(&stats->modifiers[i], value, entry->apply);
            entry++;
            i++;
        } while (i < stats->modifierCount);
    }
    return value;
}
#else
INCLUDE_ASM("asm/nonmatching", sub_0811EA34);
#endif
