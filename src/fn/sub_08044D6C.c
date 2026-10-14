// MATCHING_CONCESSION: register-pinning
#include "global.h"
u32 EncodedRecord_NextMaskWord(u32 *);
void EncodedRecord_XorRange(u8 *buffer, s32 start, s32 end, u32 seed, volatile s32 skipInput,
                            u32 mask)
{
    u32 word;
    s32 count;
    s32 skip;
    s32 index;
    skip = skipInput;
    word = 0xFFFF;
    for (count = 0; count < skip + 4; count++)
        word ^= EncodedRecord_NextMaskWord(&seed);
    index = start;
    asm("" : "+r"(index) : : "r0", "r1", "r2", "r3");
    if (index < end) {
        register u8 *base asm("r0") = buffer;
        register u8 *current asm("r5");
        s32 remaining;
        current = (void *)(index + (u32)base);
        remaining = end - index;
        do {
            u32 byte = *current;
            u32 value;
            value = word;
            asm("" : "+r"(value));
            value ^= byte;
            value &= mask;
            *current = value;
            word ^= EncodedRecord_NextMaskWord(&seed);
            current++;
            remaining--;
        } while (remaining != 0);
    }
}
