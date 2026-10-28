#include "global.h"
#include "ram.h"
struct S {
    u8 pad[0x1ad8];
    u16 values[1];
};

void sub_08111A24(s32 index, u32 value)
{
    if (((struct S *)gPaletteFade))
        ((struct S *)gPaletteFade)->values[index] = value;
}
