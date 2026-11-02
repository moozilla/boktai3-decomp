#include "global.h"
#include "player.h"
extern const u8 gUnk_08E6290C[];
void Player_InitializeActionState(void *, const void *);
void CellObject_Init(void *, void *);
void sub_08238238(void *);
u32 ActorVariant_GetTableValue(u32, u32);
void Player_SetAnimationIndex(void *, u32, u32);
void Player_SetTetherMode(void *, u32, u32);
// confidence=low: restores saved position and resource values while resetting the linked action state.
#define Player_RestoreLinkedActionState sub_0823C280
void Player_RestoreLinkedActionState(struct Player *p)
{
    void *cell;
    if (p->actionMode != 6)
        Player_InitializeActionState(p, gUnk_08E6290C);
    p->kind = 1;
    p->currentStatA = p->maximumStatA;
    p->currentStatB = p->maximumStatB;
    p->position = p->savedLinkPosition;
    cell = &p->movementContactFlags;
    CellObject_Init(cell, &p->position);
    p->movementProbe = cell;
    p->collider.cost = 0;
    p->directionFrame = 0;
    p->terrainClampBlocked = 0;
    p->positionResetValue = 90;
    p->terrainRecoveryCounter = 0;
    p->linkPositionRestored = 1;
    p->facing = p->savedLinkFacing;
    sub_08238238(p);
    Player_SetAnimationIndex(p, ActorVariant_GetTableValue(p->actionMode, 0), 64);
    Player_SetTetherMode(p, 0, 0);
}
