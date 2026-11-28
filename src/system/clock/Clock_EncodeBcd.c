#include "global.h"
s32 Mod(s32, s32);
s32 Div(s32, s32);
u8 Clock_EncodeBcd(u8 value)
{
    s32 ones = Mod(value, 10);
    return (Div(Mod(value, 100) - ones, 10) << 4) | ones;
}
