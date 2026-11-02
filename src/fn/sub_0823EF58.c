#include "player.h"

s32 ActorVariant_GetTableValue(u32, s32);
s32 Player_SetAnimationIndex(u8 *, s32, s32);
void Player_UpdateFacingFromInputHistory(u8 *p);
void sub_0823EDDC(u8 *p);
s32 sub_0823EE80(u8 *p);
void Player_SetTetherMode(u8 *, s32, s32);

void Player_UpdateTetherRecall(u8 *p)
{
    switch (((struct Player *)p)->tetherPhase) {
        case 0:
            if (Player_SetAnimationIndex(
                    p, ActorVariant_GetTableValue(((struct Player *)p)->actionMode, 0x20), 0x40)) {
                if (((struct Player *)p)->inputKeys[0] & 2) {
                    Player_SetTetherMode(p, 3, 1);
                } else {
                    sub_0823EDDC(p);
                    Player_SetTetherMode(p, 3, 4);
                }
            }
            break;
        case 1:
            Player_UpdateFacingFromInputHistory(p);
            Player_SetAnimationIndex(
                p, ActorVariant_GetTableValue(((struct Player *)p)->actionMode, 0x21), 0x40);
            if (!(((struct Player *)p)->inputKeys[0] & 2)) {
                sub_0823EDDC(p);
                Player_SetTetherMode(p, 3, 4);
            } else {
                ((struct Player *)p)->tetherFrame += 1;
            }
            break;
        case 4:
            *(u8 *)(p + 0x4B9) = 0;
            Player_SetAnimationIndex(
                p, ActorVariant_GetTableValue(((struct Player *)p)->actionMode, 0x22), 0x40);
            if (((struct Player *)p)->inputKeys[1] & 2)
                *(u8 *)(p + 0xBF6) = 1;
            if (sub_0823EE80(p))
                Player_SetTetherMode(p, 3, 5);
            break;
        case 5:
            *(u8 *)(p + 0x4B9) = 0;
            Player_SetAnimationIndex(
                p, ActorVariant_GetTableValue(((struct Player *)p)->actionMode, 0x21), 0x40);
            if (((struct Player *)p)->inputKeys[1] & 2)
                *(u8 *)(p + 0xBF6) = 1;
            {
                u32 released = *(u8 *)(p + 0xBF6);
                if (released) {
                    *(u8 *)(p + 0xBF6) = 0;
                    ((struct Player *)p)->tetherPhase = 1;
                } else {
                    ((struct Player *)p)->tetherFrame += 1;
                    if (((struct Player *)p)->tetherFrame > 9) {
                        *(u8 *)(p + 0xBF6) = 0;
                        Player_SetTetherMode(p, 3, 6);
                    }
                }
            }
            break;
        case 6:
            *(u8 *)(p + 0x4B9) = 0;
            Player_SetAnimationIndex(
                p, ActorVariant_GetTableValue(((struct Player *)p)->actionMode, 0x20), 0x40);
            ((struct Player *)p)->tetherFrame += 1;
            if (((struct Player *)p)->tetherFrame > 4)
                Player_SetTetherMode(p, 0, 0);
            break;
    }
}
