#define ModifierStats_ApplyKind0 sub_0811E8B0
#define Callback_InvokeR2 sub_08249240
#include "global.h"

/* Tentative stat/modifier layout. The array is a partial view, not a recovered capacity. */

struct StatModifier {
    s32 (*apply)(struct StatModifier *, s32);
    u8 unknown4;
    u8 kind;
    u8 unknown6[14];
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
s32 sub_0811E8B0(struct ModifierStats *stats, s32 value)
{
    struct StatModifier *modifier = stats->modifiers;
    s32 i = 0;
    if (i < stats->modifierCount) {
        struct StatModifier *entry = modifier;
        do {
            if (entry->kind == 0)
                value = sub_08249240(modifier, value, entry->apply);
            entry++;
            modifier++;
            i++;
        } while (i < stats->modifierCount);
    }
    return value;
}
