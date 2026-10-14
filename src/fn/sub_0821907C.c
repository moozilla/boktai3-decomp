#include "global.h"
#include "engine.h"

extern u32 gUnk_03005180[];

void sub_0821907C(u32 mode)
{
    u32 key;
    u8 *res;
    switch (mode) {
        case 0:
        default:
            key = 0x529c;
            break;
        case 1:
            key = 0x539c;
            break;
    }
    res = ScriptProc_Start(0x92b3, key);
    CpuSet(res + 0x1d4, gUnk_03005180, 0x04000010);
}
