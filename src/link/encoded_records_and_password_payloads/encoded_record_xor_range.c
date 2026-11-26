#include "engine/shared/global.h"
u32 EncodedRecord_NextMaskWord(u32 *);
void EncodedRecord_XorRange(u8 *buffer, s32 start, s32 end, u32 seed, s32 skip, u32 mask)
{
    u32 word = 0xFFFF;
    s32 i;

    for (i = 0; i < skip + 4; i++)
        word ^= EncodedRecord_NextMaskWord(&seed);
    for (i = start; i < end; i++) {
        buffer[i] ^= word;
        buffer[i] &= mask;
        word ^= EncodedRecord_NextMaskWord(&seed);
    }
}
