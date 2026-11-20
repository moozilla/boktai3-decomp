#include "global.h"
#include "player.h"
#include "ram.h"
#include "player_timed_cue.h"
void Player_EmitQueuedEffects(struct Player *);
void Player_StopActionSounds(struct Player *);
void Player_RestoreCameraFollow(struct Player *);
void Player_EnterKindFour(struct Player *p)
{
    s32 i;
    if ((u8)(p->actionMode - 3) <= 2) {
        p->actionLatch464 = 1;
        p->actionLatch465 = 1;
    }
    p->facing |= 1;
    if (p->facing > 4) {
        p->heading = (8 - p->facing) >> 1;
        p->side = 1;
    } else {
        p->heading = p->facing >> 1;
        p->side = 0;
    }
    p->kind = 4;
    p->tetherPhase = 0;
    p->tetherFrame = 0;
    p->positionResetValue = 0;
    p->actionSelectionStamp = 0;
    gPlayerSolarActionPending = 0;
    Player_SetTimedCue(0, 0);
    Player_EmitQueuedEffects(p);
    p->sceneMode = 0;
    p->unk536 = 0;
    p->active = 0;
    for (i = 0; i < 2; i++)
        p->scriptStatus[i] = 0;
    Player_StopActionSounds(p);
    if (p->cameraOffsetActive)
        Player_RestoreCameraFollow(p);
}
