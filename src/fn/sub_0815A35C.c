#include "global.h"
#include "render.h"

struct O0815A35C {
    u8 f0[0xD4];
    u32 fD4;
};
extern struct O0815A35C *gUnk_02000580;

u32 Player_GetCollisionData(void)
{
    if (gUnk_02000580 == 0)
        return (u32)(gPaletteBankData + 0x540);
    return gUnk_02000580->fD4;
}
