
#ifdef NONMATCHING
// fuzzy: 86.7%
// foundry draft (worker best 86.2%); what differs is mostly register allocation and instruction order
#define ModifierStats_ApplyKind15 sub_0811E930
#include "global.h"

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
s32 sub_0811E930(struct ModifierStats *stats, s32 value)
{
    u32 modifier = (u32)stats->modifiers;
    s32 i = 0;
    if (i < stats->modifierCount) {
        do {
            struct StatModifier *entry = &stats->modifiers[i];
            if (entry->kind == 15)
                value = sub_08249240((void *)modifier, value, entry->apply);
            i++;
            modifier += 0x14;
        } while (i < stats->modifierCount);
    }
    return value;
}
#else
INCLUDE_ASM("asm/nonmatching", sub_0811E930);
#endif
