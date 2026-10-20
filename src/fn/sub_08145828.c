#include "global.h"
#include "player.h"

void Sound_PlaySong(u32);
s32 ActorVariant_GetTableValue(u32, s32);
s32 Player_SetAnimation(void *, s32, s32, s32);
void ActorSequence_Begin(void *, s32, s32);
u32 Player_ConsumeStateEntry(struct Player *);
void Player_SetActionPhase(struct Player *, u8);
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
// confidence=low: recoil phases end by clearing the special contact state and returning to idle.
#define Player_UpdateContactKnockdown sub_08145828
void Player_UpdateContactKnockdown(struct Player *p)
{
    s32 value, angle;
    if (Player_ConsumeStateEntry(p)) {
        if (p->facing > 4) {
            p->heading = (8 - p->facing) >> 1;
            p->side = 1;
        } else {
            p->heading = p->facing >> 1;
            p->side = 0;
        }
        if (p->contactActionTag) {
            Sound_PlaySong(p->contactActionTag);
            p->contactActionTag = 0;
        }
    }
    {
        u32 mask = 0x40000;
        p->statusFlags |= mask;
    }
    switch (p->tetherPhase) {
        case 0:
            Player_SetAnimation(p, ActorVariant_GetTableValue(p->actionMode, 0x29), 0x40, 1);
            if (++p->tetherFrame > 2)
                Player_SetActionPhase(p, 1);
            break;
        case 1:
            if (!p->tetherFrame)
                p->terrainRecoveryCounter = 300;
            Player_SetAnimation(p, ActorVariant_GetTableValue(p->actionMode, 0x2B), 0x40, 1);
            angle = ((p->facing + 5) & 7) << 5;
            value = gUnk_086149C4[(angle + 64) & 255] * p->directionFrame;
            p->velocityX = -scale(value);
            value = gUnk_086149C4[angle] * p->directionFrame;
            p->velocityZ = -scale(value);
            p->directionFrame = (p->directionFrame * 3) >> 2;
            if (p->tetherFrame > 3) {
                if (p->terrainRecoveryCounter >= p->tetherFrame * 8)
                    p->terrainRecoveryCounter -= p->tetherFrame * 8;
                else
                    p->terrainRecoveryCounter = 0;
            }
            p->tetherFrame++;
            if (p->directionFrame <= 19 && p->terrainRecoveryCounter == 0) {
                p->terrainRecoveryCounter = 0;
                Sound_PlaySong(0x2A8);
                Player_SetActionPhase(p, 2);
            }
            break;
        case 2:
            angle = ((p->facing + 5) & 7) << 5;
            value = gUnk_086149C4[(angle + 64) & 255] * p->directionFrame;
            p->velocityX = -scale(value);
            value = gUnk_086149C4[angle] * p->directionFrame;
            p->velocityZ = -scale(value);
            if (Player_SetAnimation(p, ActorVariant_GetTableValue(p->actionMode, 0x2C), 0x40, 1)) {
                p->directionFrame = 0;
                Player_SetActionPhase(p, 3);
            }
            break;
        case 3:
            if (++p->tetherFrame > 19)
                Player_SetActionPhase(p, 4);
            break;
        case 4:
            if (Player_SetAnimation(p, ActorVariant_GetTableValue(p->actionMode, 0x2D), 0x40, 1)) {
                p->contactSpecial = 0;
                Player_UpdateFacingSide(p);
                ActorSequence_Begin(p, 0, 0);
            }
            break;
    }
}
