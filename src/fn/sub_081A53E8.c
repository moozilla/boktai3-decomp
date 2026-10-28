#include "global.h"
#include "input.h"
static inline u32 tst(volatile u16 *a, u32 m)
{
    return *a & m;
}
void Debug_NudgeScreenPositionWithB(u16 *p)
{
    if ((*(u16 *)gPadState) & 2) {
        if ((*(u16 *)gPadState) & 0x10)
            p[0] += 2;
        else if ((*(u16 *)gPadState) & 0x20)
            p[0] -= 2;
        if ((*(u16 *)gPadState) & 0x40)
            p[1] -= 2;
        else if ((*(u16 *)gPadState) & 0x80)
            p[1] += 2;
    }
}
