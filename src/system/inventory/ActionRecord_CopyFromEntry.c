#define gItemDescriptorDefaults gUnk_08E6092C
#include "global.h"
struct ItemDescriptorView {
    u8 type;
    u8 unk1[11];
    u32 values[3];
    u8 unk18[8];
};
struct ItemRecord {
    u8 type;
    u8 unk1[3];
    u32 values[3];
};
extern const struct ItemDescriptorView gUnk_08E6092C[];
void ActionRecord_CopyFromEntry(struct ItemRecord *record, struct ItemDescriptorView *data)
{
    if (record->type == 255)
        data->type = 255;
    else {
        s32 i;
        struct ItemRecord *src;
        struct ItemDescriptorView *dst;
        *data = gUnk_08E6092C[record->type];
        src = record;
        dst = data;
        i = 2;
        do {
            dst->values[0] = src->values[0];
            src = (void *)((u8 *)src + 4);
            dst = (void *)((u8 *)dst + 4);
        } while (--i >= 0);
    }
}
