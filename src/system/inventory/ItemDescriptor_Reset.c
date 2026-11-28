#include "ui/widgets/scene_input_compare.h"
struct ItemDescriptorValue {
    u8 bytes[4];
};
struct ItemDescriptorPackedView {
    u8 type, unk1, field2, field3, field4, unk5;
    u16 field6;
    u8 field8, unk9[3];
    struct ItemDescriptorValue values[3];
    u8 fields18[7];
    u8 field1F;
};
void ItemDescriptor_Reset(struct SceneInputItemDescriptor *descriptor)
{
    struct ItemDescriptorPackedView *record = (void *)descriptor;
    s32 i, j;

    record->type = 255;
    record->field2 = 0;
    record->field3 = 0;
    record->field4 = 0;
    record->field6 = 0;
    for (i = 0; i < 3; i++) {
        record->values[i].bytes[0] = 0;
        record->values[i].bytes[1] = 0;
        record->values[i].bytes[2] = 0;
        record->values[i].bytes[3] = 0;
    }
    record->field8 = 0;
    for (i = 0; i < 7; i++)
        record->fields18[i] = 0;
    record->field1F = 0;
}
