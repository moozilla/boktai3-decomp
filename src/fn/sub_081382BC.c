#include "global.h"
#define ByteSequence_AdvanceEncodedValue sub_081382BC

struct ByteSequence {
    u8 unknown0[0x3E];
    u16 encodedValue;
    u8 unknown40[4];
    s32 status;
    u8 unknown48[0x32];
    u16 index;
};
s32 sub_081382BC(struct ByteSequence *sequence, const u8 *bytes, s32 count)
{
    if (sequence->index < count) {
        if (sequence->status < 0)
            return 0;
        {
            u32 value = bytes[sequence->index] | ((sequence->index & 3) << 8);
            value |= 0x800;
            sequence->encodedValue = value;
        }
        sequence->index++;
        if (sequence->index < count)
            return 0;
    }
    return 1;
}
