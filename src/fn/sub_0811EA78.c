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
    u8 unknown55[0x7B];
    struct StatModifier modifiers[32];
};
s32 sub_08249240(void *, s32, s32 (*)(struct StatModifier *, s32));

// Folds value through every modifier whose kind matches (kind == 1).
s32 sub_0811EA78(struct ModifierStats *stats, s32 value)
{
    struct StatModifier *base = (*&stats)->modifiers;
    s32 i = 0;
    if (i < stats->modifierCount) {
        struct StatModifier *modifier = base;
        struct StatModifier *entry = base;
        do {
            if (entry->kind == 1)
                value = sub_08249240(modifier, value, entry->apply);
            entry++;
            modifier++;
            i++;
        } while (i < stats->modifierCount);
    }
    return value;
}
