#include "global.h"
#include "player.h"

void Sound_PlaySong(u32);
s32 ActorVariant_GetTableValue(u32, s32);
s32 Player_SetAnimation(void *, s32, s32, s32);
void ActorSequence_Begin(void *, s32, s32);
void PlayerCue_EnterStandUpdate(void *);
void PlayerCue_CompleteAction(void *);
void Player_UpdateFacingSide(void *);
extern const s16 gUnk_086149C4[];

static inline s32 scale(s32 v)
{
    if (v >= 0)
        return v >> 12;
    return -((-v) >> 12);
}
// confidence=low: cue motion 0x1E recoils, waits, then recovers unless requested to stay down.
void PlayerCue_UpdateKnockdownRecovery(struct Player *p)
{
    s32 value, angle;
    switch (p->tetherPhase) {
        case 0:
            Player_SetAnimation(p, ActorVariant_GetTableValue(p->actionMode, 0x29), 0x40, 1);
            if (++p->tetherFrame > 2)
                ActorSequence_Begin(p, 0x1E, 1);
            break;
        case 1:
            if (!p->tetherFrame)
                p->terrainRecoveryCounter = 300;
            Player_SetAnimation(p, ActorVariant_GetTableValue(p->actionMode, 0x2B), 0x40, 1);
            angle = ((p->facing + 5) & 7) << 5;
            value = gUnk_086149C4[(angle + 64) & 255] * p->recoilSpeed;
            p->velocityX = -scale(value);
            value = gUnk_086149C4[angle] * p->recoilSpeed;
            p->velocityZ = -scale(value);
            p->recoilSpeed = (p->recoilSpeed * 3) >> 2;
            if (p->tetherFrame > 3) {
                if (p->terrainRecoveryCounter >= p->tetherFrame * 8)
                    p->terrainRecoveryCounter -= p->tetherFrame * 8;
                else
                    p->terrainRecoveryCounter = 0;
            }
            p->tetherFrame++;
            if (p->recoilSpeed <= 19 && p->terrainRecoveryCounter == 0) {
                p->terrainRecoveryCounter = 0;
                Sound_PlaySong(0x2A8);
                ActorSequence_Begin(p, 0x1E, 2);
            }
            break;
        case 2:
            angle = ((p->facing + 5) & 7) << 5;
            value = gUnk_086149C4[(angle + 64) & 255] * p->recoilSpeed;
            p->velocityX = -scale(value);
            value = gUnk_086149C4[angle] * p->recoilSpeed;
            p->velocityZ = -scale(value);
            if (Player_SetAnimation(p, ActorVariant_GetTableValue(p->actionMode, 0x2C), 0x40, 1)) {
                p->recoilSpeed = 0;
                ActorSequence_Begin(p, 0x1E, 3);
            }
            break;
        case 3:
            if (++p->tetherFrame > 19) {
                if (p->animationBranchRequested) {
                    p->animationBranchRequested = 0;
                    ActorSequence_Begin(p, 0x1E, 5);
                    PlayerCue_CompleteAction(p);
                } else
                    ActorSequence_Begin(p, 0x1E, 4);
            }
            break;
        case 4:
            if (!p->tetherFrame) {
                Sound_PlaySong(0xDB);
                p->tetherFrame++;
            }
            if (Player_SetAnimation(p, ActorVariant_GetTableValue(p->actionMode, 0x2D), 0x40, 1)) {
                Sound_PlaySong(0xD0);
                Player_UpdateFacingSide(p);
                PlayerCue_EnterStandUpdate(p);
                PlayerCue_CompleteAction(p);
            }
            break;
        case 5:
            break;
    }
}
