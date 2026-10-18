#define ItemDescriptor_Reset sub_0822D7C0
#include "scene_input_compare.h"
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
void sub_0822D7C0(struct SceneInputItemDescriptor *descriptor)
{
    struct ItemDescriptorPackedView *record = (void *)descriptor;
    struct ItemDescriptorValue *value;
    u8 zero;
    u8 *begin;
    s32 i;
    u8 *p;
    record->type = 255;
    record->field2 = 0;
    record->field3 = 0;
    record->field4 = 0;
    record->field6 = 0;
    zero = 0;
    value = record->values;
    i = 2;
    do {
        value->bytes[0] = zero;
        value->bytes[1] = zero;
        value->bytes[2] = zero;
        value->bytes[3] = zero;
        value++;
    } while (--i >= 0);
    record->field8 = 0;
    begin = record->fields18;
    {
        u8 lastZero = 0;
        p = record->fields18 + 6;
        do {
            *p-- = lastZero;
        } while ((s32)p >= (s32)begin);
    }
    record->field1F = 0;
}
