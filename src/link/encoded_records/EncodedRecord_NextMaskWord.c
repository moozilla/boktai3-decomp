#include "global.h"

u32 EncodedRecord_NextMaskWord(u32 *p)
{
    return *p = *p * 0x6262C05D + 1;
}
