#include "global.h"
extern u8 *gUnk_02000610;
u8 *Script_GetPc(void)
{
    u8 *p = gUnk_02000610;
    if (p == 0 || *p == 0 || (*p & 0xF0) == 0x50) return 0;
    return p;
}
