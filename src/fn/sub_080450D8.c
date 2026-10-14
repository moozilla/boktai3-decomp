#define EncodedRecord_DecodeFields sub_080450D8
#include "encoded_record.h"
void EncodedRecord_DecodeFields(struct EncodedRecordFields *fields, u8 *record, s32 skip)
{
    u32 bit;
    s32 i;
    EncodedRecord_XorRange(record, 4, 24, 0x8C159, skip, 63);
    bit = 24;
    fields->sessionValue = EncodedRecord_ReadBits(record, &bit, 6, 6);
    fields->counter538 = EncodedRecord_ReadBits(record, &bit, 8, 6) << 3;
    fields->value664 = EncodedRecord_ReadBits(record, &bit, 7, 6);
    fields->value75A = EncodedRecord_ReadBits(record, &bit, 9, 6);
    fields->value7B4 = EncodedRecord_ReadBits(record, &bit, 7, 6);
    fields->value75F = EncodedRecord_ReadBits(record, &bit, 1, 6);
    fields->value75E = EncodedRecord_ReadBits(record, &bit, 6, 6);
    fields->value520 = EncodedRecord_ReadBits(record, &bit, 10, 6) << 12;
    fields->value51C = EncodedRecord_ReadBits(record, &bit, 8, 6) << 8;
    fields->elapsedUnits = EncodedRecord_ReadBits(record, &bit, 6, 6);
    fields->value536 = EncodedRecord_ReadBits(record, &bit, 12, 6);
    for (i = 0; i <= 4; i++)
        fields->name[i] = EncodedRecord_ReadBits(record, &bit, 8, 6);
}
