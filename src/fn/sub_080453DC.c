#define EncodedRecord_DecodeAlternateFields sub_080453DC
#define EncodedRecord_ReadBits              sub_08044CF4
#define EncodedRecord_XorRange              sub_08044D6C
#include "encoded_record.h"
void EncodedRecord_DecodeAlternateFields(struct EncodedAlternateRecordFields *fields, u8 *record,
                                         s32 skip)
{
    u32 bit;
    s32 i;
    EncodedRecord_XorRange(record, 4, 24, 0xB8E6E, skip, 63);
    bit = 24;
    fields->sessionValue = EncodedRecord_ReadBits(record, &bit, 6, 6);
    fields->value660 = EncodedRecord_ReadBits(record, &bit, 3, 6);
    fields->value662 = EncodedRecord_ReadBits(record, &bit, 4, 6);
    fields->counter538 = EncodedRecord_ReadBits(record, &bit, 8, 6) << 3;
    fields->value664 = EncodedRecord_ReadBits(record, &bit, 7, 6);
    fields->value7B4 = EncodedRecord_ReadBits(record, &bit, 6, 6);
    fields->reserved18 = EncodedRecord_ReadBits(record, &bit, 6, 6);
    fields->value520 = EncodedRecord_ReadBits(record, &bit, 10, 6) << 12;
    fields->value51C = EncodedRecord_ReadBits(record, &bit, 8, 6) << 8;
    fields->elapsedUnits = EncodedRecord_ReadBits(record, &bit, 6, 6);
    fields->value536 = EncodedRecord_ReadBits(record, &bit, 16, 6);
    for (i = 0; i <= 4; i++)
        fields->name[i] = EncodedRecord_ReadBits(record, &bit, 8, 6);
}
