#include "global.h"
struct EquipmentEffectEntry {
    u8 kind;
    u8 unk1[11];
};
struct EquipmentEffects {
    u8 unk0[4];
    struct EquipmentEffectEntry entries[5];
};
extern struct EquipmentEffects gUnk_08E5FA6C[];

/* Count the leading used entries (up to five) of the effect record for equipment p[0xd]. */
s32 Inventory_CountEquipmentEffectEntries(u8 *p)
{
    struct EquipmentEffects *record = &gUnk_08E5FA6C[p[0xd]];
    s32 count = 0;
    s32 i = 0;
    struct EquipmentEffectEntry *entry = record->entries;

    do {
        if (entry->kind == 0)
            break;
        count++;
        entry++;
        i++;
    } while (i < 5);
    return count;
}
