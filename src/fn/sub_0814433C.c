#include "global.h"
#include "player.h"
void Player_PlayPositionEffect(struct Player *, struct SceneVec *, s32, s32, s32, s32, s32);
void Player_UpdateFacingSide(struct Player *);
s32 ActorVariant_GetTableValue(u32, s32);
s32 Player_SetAnimation(struct Player *, s32, s32, s32);
void ActorSequence_Begin(struct Player *, s32, s32);
void Player_UpdateMovementFacing(struct Player *p)
{
    if ((s16)p->directionHistory[0] >= 0 && p->facing != (s16)p->directionHistory[0]) {
        if (p->facingChangeEffects & 1) {
            if ((p->facingChangeEffects >> 1) & 1)
                Player_PlayPositionEffect(p, &p->position, 1, 1, 0, 0, 0x116);
            else
                Player_PlayPositionEffect(p, &p->position, 1, 1, 0, 0, 0x117);
        }
        p->facing = (s16)p->directionHistory[0];
        Player_UpdateFacingSide(p);
    }
    Player_SetAnimation(p, ActorVariant_GetTableValue(p->actionMode, 0), 64, 1);
    if ((s8)p->movementAnimationState < 0)
        ActorSequence_Begin(p, 0, 0);
}
