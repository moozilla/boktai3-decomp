#include "player/player.h"

s32 ActorVariant_GetTableValue(u32, s32);
s32 Player_SetAnimationIndex(struct Player *, s32, s32);
void Player_UpdateFacingFromInputHistory(struct Player *p);
void sub_0823EDDC(struct Player *p);
s32 sub_0823EE80(struct Player *p);
void Player_SetTetherMode(struct Player *, s32, s32);

void Player_UpdateTetherRecall(struct Player *p)
{
    switch (p->tetherPhase) {
        case 0:
            if (Player_SetAnimationIndex(p, ActorVariant_GetTableValue(p->actionMode, 0x20),
                                         0x40)) {
                if (p->inputKeys[0] & 2) {
                    Player_SetTetherMode(p, 3, 1);
                } else {
                    sub_0823EDDC(p);
                    Player_SetTetherMode(p, 3, 4);
                }
            }
            break;
        case 1:
            Player_UpdateFacingFromInputHistory(p);
            Player_SetAnimationIndex(p, ActorVariant_GetTableValue(p->actionMode, 0x21), 0x40);
            if (!(p->inputKeys[0] & 2)) {
                sub_0823EDDC(p);
                Player_SetTetherMode(p, 3, 4);
            } else {
                p->tetherFrame++;
            }
            break;
        case 4:
            p->cameraInputProcessed = 0;
            Player_SetAnimationIndex(p, ActorVariant_GetTableValue(p->actionMode, 0x22), 0x40);
            if (p->inputKeys[1] & 2)
                p->statModifierContext.bufferedAttack = 1;
            if (sub_0823EE80(p))
                Player_SetTetherMode(p, 3, 5);
            break;
        case 5:
            p->cameraInputProcessed = 0;
            Player_SetAnimationIndex(p, ActorVariant_GetTableValue(p->actionMode, 0x21), 0x40);
            if (p->inputKeys[1] & 2)
                p->statModifierContext.bufferedAttack = 1;
            if (p->statModifierContext.bufferedAttack) {
                p->statModifierContext.bufferedAttack = 0;
                p->tetherPhase = 1;
            } else {
                p->tetherFrame++;
                if (p->tetherFrame > 9) {
                    p->statModifierContext.bufferedAttack = 0;
                    Player_SetTetherMode(p, 3, 6);
                }
            }
            break;
        case 6:
            p->cameraInputProcessed = 0;
            Player_SetAnimationIndex(p, ActorVariant_GetTableValue(p->actionMode, 0x20), 0x40);
            p->tetherFrame++;
            if (p->tetherFrame > 4)
                Player_SetTetherMode(p, 0, 0);
            break;
    }
}
