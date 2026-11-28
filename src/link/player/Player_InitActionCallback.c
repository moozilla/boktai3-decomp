#include "global.h"
#include "player/player.h"
void Player_UpdateActionPhase(struct Player *);
void Player_UpdateLinkKind(struct Player *);
void Player_SetTetherMode(struct Player *, u32, u32);
s32 Player_GetActionIndex(struct Player *);
u32 LinkPlayer_IsAbilityIdValid(struct Player *, s32);
void Player_InitActionCallback(struct Player *p)
{
    if (p->actionLatch464 == 0)
        return;
    if (p->actionLatch465 == 3)
        return;
    if (p->actionLatch465 == 5)
        return;
    if (p->actionLatch465 == 4) {
        p->actionMode = 4;
        p->updateCallback = Player_UpdateActionPhase;
        p->unk419 = 1;
        p->activeAnimation = NULL;
        p->visualState = (u32)&p->secondaryDrawFlags;
        p->visualPosition = &p->colliderPosition;
        p->savedSpriteResource = 0xFFFF;
        Player_SetTetherMode(p, 0, 0);
        p->actionEighteenParameter = 0x16F;
        p->actionEighteenDuration = 0x20;
    } else {
        p->actionMode = 1;
        p->updateCallback = Player_UpdateLinkKind;
        p->unk419 = 0;
        p->activeAnimation = &p->animation;
        p->visualState = 0;
        p->visualPosition = &p->animation.position;
        p->savedSpriteResource = 0xFFFF;
        p->actionEighteenParameter = 0x16F;
        p->actionEighteenDuration = 0x20;
        p->actionVariant = Player_GetActionIndex(p) + 1;
        p->baseActionVariant = p->actionVariant + 1;
    }
    p->actionEnabled = LinkPlayer_IsAbilityIdValid(p, (s8)p->thresholdMode);
    p->actionLatch464 = 0;
}
