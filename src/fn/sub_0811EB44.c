#include "global.h"
#include "modifier_list.h"
struct ModifierSourceIds {
    u8 unknown0[8];
    s8 ids[4];
    u8 count;
};
struct ModifierActor {
    u8 unknown0[0x48];
    struct ModifierSourceIds sources;
    u8 unknown58[0x78];
    struct StatModifier modifiers[24];
};
s32 sub_0811E728(u8 *, s32);
s32 sub_0811E77C(u8 *, s32);
s32 Modifier_ScaleKindSevenStat(u8 *, s32);
s32 Modifier_AddKindThreeStat(u8 *, s32);
s32 Modifier_ScaleKindFourStat(u8 *, s32);
s32 sub_0811E7C8(u8 *, s32);
s32 Modifier_AddKindSixStat(u8 *, s32);
s32 Modifier_ScaleKindEightToTenStat(u8 *, s32);
s32 StatModifier_ScaleAtOwnerThreshold(u8 *, s32);
s32 Modifier_ScaleDefaultStat(u8 *, s32);
void StatModifier_InitializeCallbacks(struct ModifierActor *p, struct StatModifierOwner *owner)
{
    struct ModifierSourceIds *sources = &p->sources;
    s32 i;
    struct StatModifier *slots = p->modifiers;
    {
        u32 zero = 0;
        struct ModifierRecord *record = (struct ModifierRecord *)&p->modifiers[0].order;
        struct StatModifier *slot = slots;
        s32 remaining = 23;
        do {
            slot->callback = (void *)zero;
            *((u8 *)record + 8) = zero;
            *(struct StatModifierOwner **)((u8 *)record + 12) = owner;
            Memory_Clear(record, 8);
            record = (struct ModifierRecord *)((u8 *)record + 20);
            slot++;
        } while (--remaining >= 0);
    }
    for (i = 0; i < sources->count; i++) {
        ScriptRecord_FindModifierRecord((struct ModifierRecord *)&slots[i].order, sources->ids[i]);
        switch (slots[i].kind) {
            case 1:
                slots[i].callback = sub_0811E728;
                slots[i].enabled = 1;
                break;
            case 2:
                slots[i].callback = sub_0811E77C;
                slots[i].enabled = 1;
                break;
            case 7:
                slots[i].callback = Modifier_ScaleKindSevenStat;
                slots[i].enabled = 1;
                break;
            case 3:
                slots[i].callback = Modifier_AddKindThreeStat;
                slots[i].enabled = 1;
                break;
            case 4:
                slots[i].callback = Modifier_ScaleKindFourStat;
                slots[i].enabled = 1;
                break;
            case 5:
                slots[i].callback = sub_0811E7C8;
                slots[i].enabled = 1;
                break;
            case 6:
                slots[i].callback = Modifier_AddKindSixStat;
                slots[i].enabled = 1;
                break;
            case 8:
            case 9:
            case 10:
                slots[i].callback = Modifier_ScaleKindEightToTenStat;
                slots[i].enabled = 1;
                break;
            case 13:
                slots[i].callback = StatModifier_ScaleAtOwnerThreshold;
                slots[i].enabled = 1;
                break;
            case 14:
                slots[i].callback = StatModifier_ScaleAtOwnerThreshold;
                slots[i].enabled = 1;
                break;
            case 0:
            case 11:
            case 12:
            case 15:
                slots[i].callback = Modifier_ScaleDefaultStat;
                slots[i].enabled = 1;
                break;
            default:
                continue;
        }
    }
}
