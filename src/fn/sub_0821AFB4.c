#include "global.h"
struct VM {
    u8 p0[4];
    u32 f4;
};
extern struct VM gUnk_02000610;
s32 Script_ExecuteBlock(u32, u32, u32);
u32 Script_RunBlock(u32 a, u32 b)
{
    if (Script_ExecuteBlock(a, b, 0) == 1)
        return gUnk_02000610.f4;
    return gUnk_02000610.f4 = 0;
}
