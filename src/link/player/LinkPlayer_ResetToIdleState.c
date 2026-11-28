#include "global.h"
#include "player/player.h"

/* tentative: link-player variant of entering kind four: resets to idle and updates link counters */

void Player_UpdateLinkKind(struct Player *);
void LinkPlayer_FlushPendingEffects(struct Player *);
void Player_StopLocalActionSounds(struct Player *);
void Player_ClearMovementLock(struct Player *);
void LinkSession_CallPositionScript(void *, s32);
s32 LinkSession_IncrementAlternateCounter(s32);
s32 LinkSession_IncrementCounter(s32);
void Player_NotifyFirstSelectedSlot(s32, u32);
void LinkPlayer_ResetToIdleState(struct Player *p)
{
    s32 i;
    if (p->actionMode == 4) {
        p->actionMode = 1;
        p->updateCallback = (void *)Player_UpdateLinkKind;
        p->unk419 = 0;
        p->activeAnimation = &p->animation;
        p->visualState = 0;
        p->visualPosition = (struct SceneVec *)&p->animation.position;
        p->savedSpriteResource = 0xFFFF;
    }
    p->facing |= 1;
    if (p->facing > 4) {
        p->heading = (8 - p->facing) >> 1;
        p->side = 1;
    } else {
        p->heading = p->facing >> 1;
        p->side = 0;
    }
    p->kind = 8;
    p->tetherPhase = 0;
    p->tetherFrame = 0;
    p->positionResetValue = 0;
    p->actionSelectionStamp = 0;
    LinkPlayer_FlushPendingEffects(p);
    Player_StopLocalActionSounds(p);
    if (p->cameraOffsetActive)
        Player_ClearMovementLock(p);
    p->sceneMode = 0;
    p->unk536 = 0;
    p->active = 0;
    p->unk4E1[0] = 0;
    p->movementVelocityLocked = 0;
    for (i = 0; i < 2; i++)
        p->scriptStatus[i] = 0;
    p->catalogSavedOption = p->actionConfig->catalogOption;
    if (p->actionMode != 6) {
        if (p->unk549 >= 0)
            LinkSession_CallPositionScript(&p->position, p->unk549);
        p->unk549 = -1;
    }
    LinkSession_IncrementAlternateCounter(p->slot);
    if (p->lastContactFlags & 0xf) {
        if (p->lastContactFlags & 1)
            LinkSession_IncrementCounter(0);
        else if (p->lastContactFlags & 2)
            LinkSession_IncrementCounter(1);
        else if (p->lastContactFlags & 4)
            LinkSession_IncrementCounter(2);
        else if (p->lastContactFlags & 8)
            LinkSession_IncrementCounter(3);
        Player_NotifyFirstSelectedSlot((s16)p->lastContactFlags, p->lastContactRewardFlags);
    }
}
