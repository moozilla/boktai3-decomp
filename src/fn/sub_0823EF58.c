#include "global.h"

// MATCHING_CONCESSION: register-pinning
s32 ActorVariant_GetTableValue(u32, s32);
s32 Player_SetAnimationIndex(u8 *, s32, s32);
void Player_UpdateFacingFromInputHistory(u8 *p);
void sub_0823EDDC(u8 *p);
s32 sub_0823EE80(u8 *p);
void Player_SetTetherMode(u8 *, s32, s32);

void Player_UpdateTetherRecall(u8 *p)
{
    switch (*(u8 *)(p + 0x458)) {
        case 0:
            if (Player_SetAnimationIndex(p, ActorVariant_GetTableValue(*(u8 *)(p + 0x418), 0x20),
                                         0x40)) {
                if (**(u16 **)(p + 0x348) & 2) {
                    Player_SetTetherMode(p, 3, 1);
                } else {
                    sub_0823EDDC(p);
                    Player_SetTetherMode(p, 3, 4);
                }
            }
            break;
        case 1:
            Player_UpdateFacingFromInputHistory(p);
            Player_SetAnimationIndex(p, ActorVariant_GetTableValue(*(u8 *)(p + 0x418), 0x21), 0x40);
            if (!(**(u16 **)(p + 0x348) & 2)) {
                sub_0823EDDC(p);
                Player_SetTetherMode(p, 3, 4);
            } else {
                *(u16 *)(p + 0x45A) += 1;
            }
            break;
        case 4:
            *(u8 *)(p + 0x4B9) = 0;
            Player_SetAnimationIndex(p, ActorVariant_GetTableValue(*(u8 *)(p + 0x418), 0x22), 0x40);
            if ((*(u16 **)(p + 0x348))[1] & 2)
                *(u8 *)(p + 0xBF6) = 1;
            if (sub_0823EE80(p))
                Player_SetTetherMode(p, 3, 5);
            break;
        case 5:
            *(u8 *)(p + 0x4B9) = 0;
            Player_SetAnimationIndex(p, ActorVariant_GetTableValue(*(u8 *)(p + 0x418), 0x21), 0x40);
            if ((*(u16 **)(p + 0x348))[1] & 2)
                *(u8 *)(p + 0xBF6) = 1;
            {
                register u32 released asm("r0") = *(u8 *)(p + 0xBF6);
                if (released) {
                    *(u8 *)(p + 0xBF6) = 0;
                    *(u8 *)(p + 0x458) = 1;
                } else {
                    *(u16 *)(p + 0x45A) += 1;
                    if (*(u16 *)(p + 0x45A) > 9) {
                        register u32 zero asm("r0") = 0;
                        *(u8 *)(p + 0xBF6) = zero;
                        Player_SetTetherMode(p, 3, 6);
                    }
                }
            }
            break;
        case 6:
            *(u8 *)(p + 0x4B9) = 0;
            Player_SetAnimationIndex(p, ActorVariant_GetTableValue(*(u8 *)(p + 0x418), 0x20), 0x40);
            *(u16 *)(p + 0x45A) += 1;
            if (*(u16 *)(p + 0x45A) > 4)
                Player_SetTetherMode(p, 0, 0);
            break;
    }
}
