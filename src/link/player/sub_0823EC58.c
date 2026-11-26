#include "global.h"

void Player_SaveActionIndex(u8 *p);
s32 ActorVariant_GetTableValue(u32, s32);
s32 Player_SetAnimationIndex(u8 *, s32, s32);
void Player_UpdateFacingFromInputHistory(u8 *p);
void sub_0823E704(u8 *p);
s32 Player_UpdateThrowArc(u8 *p);
void DirectionalEffect_Spawn(u8 *p, u32 a, u8 *b);
void Player_SetTetherMode(u8 *, s32, s32);

void Player_UpdateTetherThrow(u8 *p)
{
    switch (*(u8 *)(p + 0x458)) {
        case 0:
            Player_SaveActionIndex(p);
            if (Player_SetAnimationIndex(p, ActorVariant_GetTableValue(*(u8 *)(p + 0x418), 0x1D),
                                         0x40))
                Player_SetTetherMode(p, 3, 1);
            break;
        case 1:
            Player_SaveActionIndex(p);
            Player_UpdateFacingFromInputHistory(p);
            Player_SetAnimationIndex(p, ActorVariant_GetTableValue(*(u8 *)(p + 0x418), 0x1E), 0x40);
            if (!(**(u16 **)(p + 0x348) & 2)) {
                sub_0823E704(p);
                Player_SetTetherMode(p, 3, 4);
            } else {
                *(u16 *)(p + 0x45A) += 1;
            }
            break;
        case 4:
            *(u8 *)(p + 0x4B9) = 0;
            Player_SetAnimationIndex(p, ActorVariant_GetTableValue(*(u8 *)(p + 0x418), 0x1F), 0x40);
            if (Player_UpdateThrowArc(p))
                Player_SetTetherMode(p, 3, 5);
            break;
        case 5:
            *(u8 *)(p + 0x4B9) = 0;
            if ((*(u16 **)(p + 0x348))[1] & 2)
                *(u8 *)(p + 0xBF6) = 1;
            if (*(u8 *)(p + 0x418) == 1 && *(u16 *)(p + 0xA0) == 5 && *(u16 *)(p + 0x98) == 0)
                DirectionalEffect_Spawn(p, *(u8 *)(p + 0x3A4), p + 0x30);
            if (Player_SetAnimationIndex(p, ActorVariant_GetTableValue(*(u8 *)(p + 0x418), 0x1F),
                                         0x40)) {
                if (*(u8 *)(p + 0xBF6)) {
                    *(u8 *)(p + 0xBF6) = 0;
                    Player_SetTetherMode(p, 3, 1);
                } else {
                    *(u8 *)(p + 0xBF6) = 0;
                    Player_SetTetherMode(p, 0, 0);
                }
            }
            break;
    }
}
