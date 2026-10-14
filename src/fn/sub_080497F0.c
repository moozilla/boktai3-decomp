// MATCHING_CONCESSION: register-pinning
#include "global.h"
#define TetherActor_ClearExternalStatus sub_080497F0
struct TetherStatusOwner {
    u8 unknown0[0x5AA];
    s16 status;
};
extern struct TetherStatusOwner *gUnk_02000710;
s32 sub_0822CEE8(u32, u32);
void TetherActor_ClearExternalStatus(void)
{
    sub_0822CEE8(43, 0);
    {
        s16 *status = &gUnk_02000710->status;
        register s32 value asm("r2");
        register s32 half asm("r1");
        value = -1;
        asm("" : "+r"(value));
        half = value;
        *status = half;
    }
}
