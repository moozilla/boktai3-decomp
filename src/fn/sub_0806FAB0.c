#include "global.h"
s32 sub_0806F9D8(u16, u16);
s32 sub_0806F9AC(void);
u32 sub_0806FAB0(u16 a, u16 b)
{
    if (sub_0806F9D8(a, b) == 0 && sub_0806F9AC() != 0)
        return 1;
    return 0;
}
