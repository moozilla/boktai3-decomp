#include "global.h"
#include "player.h"

s32 ActorVariant_GetTableValue(u32, s32);
s32 Player_SetAnimationIndex(struct Player *, s32, s32);
void Player_SetTetherMode(struct Player *, s32, s32);
void Player_SaveActionIndex(struct Player *);
s32 Player_UpdateFacingFromInputHistory(struct Player *);
void Player_BeginSlashAction(struct Player *);
s32 Player_UpdateSpinAction(struct Player *);
void sub_08238CE0(struct Player *, u32, void *, u32);

void Player_UpdateSlashAction(struct Player *p)
{
    s32 speed = 0x40;
    switch (p->tetherPhase) {
        case 0:
            Player_SaveActionIndex(p);
            if (Player_SetAnimationIndex(p, ActorVariant_GetTableValue(p->actionMode, 0x18), speed))
                Player_SetTetherMode(p, 3, 1);
            break;
        case 1:
            Player_SaveActionIndex(p);
            Player_UpdateFacingFromInputHistory(p);
            Player_SetAnimationIndex(p, ActorVariant_GetTableValue(p->actionMode, 0x19), 0x40);
            if (p->tetherFrame > 6 && !(p->inputKeys[0] & 2)) {
                Player_BeginSlashAction(p);
                Player_SetTetherMode(p, 3, 4);
            } else
                p->tetherFrame++;
            break;
        case 4: {
            u8 *mode;
            u16 *kind;
            p->cameraInputProcessed = 0;
            mode = &p->actionMode;
            Player_SetAnimationIndex(
                p, ActorVariant_GetTableValue(*mode, *(kind = (u16 *)((u8 *)p + 0xC02))), speed);
            if (*mode == 1) {
                if (*kind == 0x1C) {
                    if (p->actionCueFlags == 1 && p->actionCueCounter == 0)
                        sub_08238CE0(p, p->facing, &p->position, 0);
                } else if (*kind == 0x1B) {
                    if (p->actionCueFlags == 1 && p->actionCueCounter == 0)
                        sub_08238CE0(p, p->facing, &p->position, 1);
                } else if (*kind == 0x1A) {
                    if (p->actionCueFlags == 0 && p->actionCueCounter == 1)
                        sub_08238CE0(p, p->facing, &p->position, 2);
                }
            }
            if (Player_UpdateSpinAction(p))
                Player_SetTetherMode(p, 3, 5);
            break;
        }
        case 5:
            p->cameraInputProcessed = 0;
            if (p->inputKeys[1] & 2)
                *((u8 *)p + 0xBF6) = 1;
            if (Player_SetAnimationIndex(p, ActorVariant_GetTableValue(p->actionMode, 0x18), speed))
                Player_SetTetherMode(p, 3, 6);
            break;
        case 6: {
            u8 *flag;
            u8 zero;
            p->cameraInputProcessed = 0;
            if (p->inputKeys[1] & 2)
                *((u8 *)p + 0xBF6) = 1;
            flag = (u8 *)p + 0xBF6;
            zero = *flag;
            if (zero) {
                *flag = 0;
                p->tetherPhase = 0;
            } else if (Player_SetAnimationIndex(p, ActorVariant_GetTableValue(p->actionMode, 0x19),
                                                speed)) {
                *flag = zero;
                Player_SetTetherMode(p, 0, 0);
            }
            break;
        }
    }
}
