#include "global.h"
struct VM {
    u8 p0[4];
    u32 f4;
};
extern struct VM gUnk_02000610;
extern const u8 gUnk_08614184[];
s32 Script_ExecuteBlock(u32, const u8 *, u32);
u32 Script_RunProcedureBlock(u32 a, const u8 *b, u32 c)
{
    if (b == 0)
        b = gUnk_08614184;
    if (Script_ExecuteBlock(a, b, c) == 1)
        return gUnk_02000610.f4;
    return gUnk_02000610.f4 = 0;
}
