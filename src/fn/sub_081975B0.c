#include "global.h"

void sub_08020D08(void *);
s32 sub_0819744C(void *);

s32 sub_081975B0(u8 *t)
{
    sub_08020D08(t + 0x18);
    return sub_0819744C(t);
}
