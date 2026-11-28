#include "link/encoded_records/encoded_record.h"

u32 EncodedRecord_ReadBits(const u8 *buffer, u32 *bitPos, s32 count, s32 bitsPerByte)
{
    s32 i;
    u32 result;
    s32 pos;

    result = 0;
    pos = *bitPos;
    for (i = 0; i < count; i++) {
        s32 byte = pos / bitsPerByte;
        s32 shift = pos % bitsPerByte;

        result |= ((buffer[byte] >> shift) & 1) << i;
        pos++;
    }
    *bitPos = pos;
    return result;
}
