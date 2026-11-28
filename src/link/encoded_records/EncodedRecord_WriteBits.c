#include "link/encoded_records/encoded_record.h"

void EncodedRecord_WriteBits(u8 *buffer, s32 value, u32 *bitPos, s32 count, s32 bitsPerByte)
{
    s32 i;

    if (count == 0)
        return;
    if ((u32)value > (1 << count) - 1)
        value = (1 << count) - 1;
    for (i = 0; i < count; i++) {
        s32 pos = *bitPos;
        s32 byte = pos / bitsPerByte;
        s32 shift = pos % bitsPerByte;
        s32 bit = (value >> i) & 1;

        buffer[byte] |= bit << shift;
        *bitPos = pos + 1;
    }
}
