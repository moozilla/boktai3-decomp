#include "global.h"
struct S {
    u8 pad[0x1ad8];
    u16 values[1];
};
extern struct S *gPaletteFade;

void sub_08111A24(s32 index, u32 value)
{
    if (gPaletteFade)
        gPaletteFade->values[index] = value;
}
