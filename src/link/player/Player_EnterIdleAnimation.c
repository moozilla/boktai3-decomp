#include "global.h"
#include "player/player.h"

void Player_SetTetherMode(struct Player *, s32, s32);
s32 Player_SelectHistoryDirectionB(struct Player *);
void Player_UpdateActionFacingSide(struct Player *);
s32 ActorVariant_GetTableValue(u32, s32);
void Player_SetAnimationIndex(struct Player *, s32, s32);

void Player_EnterIdleAnimation(struct Player *p)
{
    if (p->tetherMode) {
        Player_SetTetherMode(p, 0, 0);
        p->facing = Player_SelectHistoryDirectionB(p);
        Player_UpdateActionFacingSide(p);
    }
    p->statusFlags &= ~1;
    Player_SetAnimationIndex(p, ActorVariant_GetTableValue(p->actionMode, 0), 0x40);
}
