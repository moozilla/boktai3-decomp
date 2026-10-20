#include "global.h"
#include "player.h"

s32 ActorVariant_GetTableValue(u32, s32);
s32 Player_SetAnimationIndex(struct Player *, s32, s32);
void Player_SetTetherMode(struct Player *, s32, s32);
void Player_SaveActionIndex(struct Player *);
s32 Player_UpdateFacingFromInputHistory(struct Player *);
void Player_StartContactAttack(struct Player *);
s32 Player_UpdateSweepAction(struct Player *);
void Player_StartDirectionalEffect(struct Player *, u32, void *);

void Player_UpdateThrowAction(struct Player *p)
{

    switch (p->tetherPhase) {
        case 0:
            Player_SaveActionIndex(p);
            if (Player_SetAnimationIndex(p, ActorVariant_GetTableValue(p->actionMode, 0x15), 0x40))
                Player_SetTetherMode(p, 3, 1);
            break;
        case 1:
            Player_SaveActionIndex(p);
            Player_UpdateFacingFromInputHistory(p);
            Player_SetAnimationIndex(p, ActorVariant_GetTableValue(p->actionMode, 0x16), 0x40);
            if (!(p->inputKeys[0] & 2)) {
                Player_StartContactAttack(p);
                Player_SetTetherMode(p, 3, 4);
            } else
                p->tetherFrame++;
            break;
        case 4:
            p->cameraInputProcessed = 0;
            if (p->inputKeys[1] & 2)
                *((u8 *)p + 0xBF6) = 1;
            Player_SetAnimationIndex(p, ActorVariant_GetTableValue(p->actionMode, 0x17), 0x40);
            {
                if (p->actionMode == 1 && p->actionCueFlags == 2 && p->actionCueCounter == 0)
                    Player_StartDirectionalEffect(p, p->facing, &p->position);
            }
            if (Player_UpdateSweepAction(p))
                Player_SetTetherMode(p, 3, 6);
            break;
        case 6:
            p->cameraInputProcessed = 0;
            if (p->inputKeys[1] & 2)
                *((u8 *)p + 0xBF6) = 1;
            if (*((u8 *)p + 0xBF6)) {
                *((u8 *)p + 0xBF6) = 0;
                p->tetherPhase = 0;
            } else {
                p->tetherFrame++;
                if (p->tetherFrame >= p->statModifierContext.contactCounter) {
                    *((u8 *)p + 0xBF6) = 0;
                    Player_SetTetherMode(p, 0, 0);
                }
            }
            break;
    }
}
