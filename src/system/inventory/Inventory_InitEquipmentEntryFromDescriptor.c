#include "global.h"

struct ItemDescriptorView {
    u8 type;
    u8 unk1[11];
    u32 values[3];
    u8 unk18[8];
};
struct ItemRecord {
    u8 type;
    u8 unk1;
    u16 condition;
    u32 values[3];
};
void Inventory_InitEquipmentEntryFromDescriptor(const struct ItemDescriptorView *descriptor,
                                                struct ItemRecord *record)
{
    const u32 *src;
    u32 *dst;
    s32 i;

    record->type = descriptor->type;
    record->condition = 1000;
    src = (const u32 *)descriptor;
    dst = (u32 *)record;
    for (i = 0; i < 3; i++) {
        dst[1] = src[3];
        src++;
        dst++;
    }
}
