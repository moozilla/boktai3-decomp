#include "global.h"
#include "player/player.h"

void Player_SaveActionIndex(struct Player *p);
s32 ActorVariant_GetTableValue(u32, s32);
s32 Player_SetAnimationIndex(struct Player *, s32, s32);
void Player_UpdateFacingFromInputHistory(struct Player *p);
void sub_0823E704(struct Player *p);
s32 Player_UpdateThrowArc(struct Player *p);
void DirectionalEffect_Spawn(struct Player *p, u32 a, void *b);
void Player_SetTetherMode(struct Player *, s32, s32);

void Player_UpdateTetherThrow(struct Player *p)
{
    switch (p->tetherPhase) {
        case 0:
            Player_SaveActionIndex(p);
            if (Player_SetAnimationIndex(p, ActorVariant_GetTableValue(p->actionMode, 0x1D), 0x40))
                Player_SetTetherMode(p, 3, 1);
            break;
        case 1:
            Player_SaveActionIndex(p);
            Player_UpdateFacingFromInputHistory(p);
            Player_SetAnimationIndex(p, ActorVariant_GetTableValue(p->actionMode, 0x1E), 0x40);
            if (!(p->inputKeys[0] & 2)) {
                sub_0823E704(p);
                Player_SetTetherMode(p, 3, 4);
            } else {
                p->tetherFrame++;
            }
            break;
        case 4:
            p->cameraInputProcessed = 0;
            Player_SetAnimationIndex(p, ActorVariant_GetTableValue(p->actionMode, 0x1F), 0x40);
            if (Player_UpdateThrowArc(p))
                Player_SetTetherMode(p, 3, 5);
            break;
        case 5:
            p->cameraInputProcessed = 0;
            if (p->inputKeys[1] & 2)
                p->statModifierContext.bufferedAttack = 1;
            if (p->actionMode == 1 && p->animation.frameIndex == 5 && p->animation.elapsed == 0)
                DirectionalEffect_Spawn(p, p->facing, &p->position);
            if (Player_SetAnimationIndex(p, ActorVariant_GetTableValue(p->actionMode, 0x1F),
                                         0x40)) {
                if (p->statModifierContext.bufferedAttack) {
                    p->statModifierContext.bufferedAttack = 0;
                    Player_SetTetherMode(p, 3, 1);
                } else {
                    p->statModifierContext.bufferedAttack = 0;
                    Player_SetTetherMode(p, 0, 0);
                }
            }
            break;
    }
}
