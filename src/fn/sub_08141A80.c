#include "global.h"
#include "player.h"
u32 Player_ConsumeStateEntry(struct Player *);
s32 Player_SelectHistoryDirection(struct Player *);
void Player_UpdateFacingSide(struct Player *);
s32 ActorVariant_GetTableValue(u32, s32);
s32 Player_SetAnimation(struct Player *, s32, s32, s32);
void Player_BeginVariantAction(struct Player *p)
{
    if (Player_ConsumeStateEntry(p)) {
        if (p->historyDirectionMode == 1)
            p->facing = Player_SelectHistoryDirection(p);
        Player_UpdateFacingSide(p);
        p->actionResetCounter = 0;
    }
    p->statusFlags &= ~1;
    Player_SetAnimation(p, ActorVariant_GetTableValue(p->actionMode, 0), 0x40, 1);
}
