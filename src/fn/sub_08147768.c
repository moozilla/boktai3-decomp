#include "global.h"
#include "player.h"
// confidence=low: motion ten toggles actionMode between zero and one and refreshes equipment.
extern u32 gEngineFlags, gUnk_02000560;
u32 Player_ConsumeStateEntry(struct Player *);
void Player_SetPendingCallback(struct Player *, void (*)(struct Player *));
void Player_OnMotion10Exit(struct Player *);
void Player_AddControlFlags(struct Player *, s32);
void Player_SetActionPhase(struct Player *, u8);
s32 ActorVariant_GetTableValue(u32, s32);
s32 Player_SetAnimation(struct Player *, s32, s32, s32);
void Sound_PlaySong(u32);
void ActorAttachment_Create(struct Player *, s32, struct SceneVec *, s32, s32, s32);
void Player_EmitQueuedEffects(struct Player *);
void Player_NotifyActionCategories(struct Player *);
void Anim_SetKeyframes(void *, void *, s32, s32, s32);
void Anim_Step(void *, void *);
void Player_InitVisualResources(struct Player *);
void Player_UpdateFacingSide(struct Player *);
void ActorSequence_Begin(struct Player *, s32, s32);
void PlayerCue_EnterStandUpdate(struct Player *);
#define H(off) (*(u16 *)((u8 *)p + (off)))
#define B(off) (*((u8 *)p + (off)))
static inline u32 TestModeFlags(u32 mask)
{
    return gUnk_02000560 & mask;
}
void Player_UpdateFormChangeCue(struct Player *p)
{
    if (Player_ConsumeStateEntry(p)) {
        if (p->statHundredths > 999 || (gEngineFlags & 0x1000) || p->kind == 2) {
            p->actionEighteenParameter = 0x16F;
            p->actionEighteenDuration = 0;
            p->actionEighteenEnabled = 1;
        } else
            p->actionEighteenEnabled = 0;
        Sound_PlaySong(0x306);
        if (p->actionMode != 1) {
            if (p->statHundredths > 999 || (gEngineFlags & 0x1000) || p->kind == 2) {
                B(0x432) = 1;
                B(0x433) = 1;
            }
        } else {
            B(0x432) = 0;
            B(0x433) = 1;
        }
        Player_SetPendingCallback(p, Player_OnMotion10Exit);
        p->actionCountdown = 0;
        H(0x4E6) = 0;
        p->terrainRecoveryCounter = 0;
        H(0x4EA) = 0;
    }
    p->statusValue = 4000;
    if (gEngineFlags & 0x1000)
        Player_AddControlFlags(p, 8);
    {
        u32 mask = 0x100000;
        p->statusFlags = mask | p->statusFlags;
    }
    switch (p->tetherPhase) {
        case 0: {
            struct SceneVec v;
            if ((u8)(p->facing - 2) <= 4) {
                p->facing = 4;
                p->heading = 4;
                p->side = 0;
            } else {
                p->facing = 0;
                p->heading = 0;
                p->side = 0;
            }
            Player_SetAnimation(p, ActorVariant_GetTableValue(p->actionMode, 0), 64, 1);
            p->heading >>= 2;
            v.x = 0;
            v.y = 0;
            v.z = 15;
            if (p->actionEighteenEnabled || p->kind == 2)
                ActorAttachment_Create(p, 1, &v, 50, 69, 1);
            else
                ActorAttachment_Create(p, 1, &v, 50, 69, 0);
            Sound_PlaySong(0x39B);
            Player_SetActionPhase(p, 1);
            break;
        }
        case 1:
            if (p->actionEighteenEnabled) {
                if ((p->actionEighteenDuration += 3) > 32)
                    p->actionEighteenDuration = 32;
            }
            p->resetValue6FC = 15;
            if (Player_SetAnimation(p, ActorVariant_GetTableValue(p->actionMode, 8), 64, 1)) {
                if (p->statHundredths > 999 || (gEngineFlags & 0x1000) || p->actionMode == 1 ||
                    TestModeFlags(2)) {
                    Player_SetActionPhase(p, 2);
                    p->statusValue = 4000;
                    if (p->actionMode != 1) {
                        Sound_PlaySong(0x4F7);
                        Player_EmitQueuedEffects(p);
                        Player_NotifyActionCategories(p);
                    }
                } else
                    Player_SetActionPhase(p, 3);
            }
            break;
        case 2:
            if ((p->tetherFrame & 7) < 7 - (p->tetherFrame >> 2))
                Player_SetAnimation(p, ActorVariant_GetTableValue(p->actionMode, 10), 64, 1);
            else if (p->actionMode != 1) {
                Anim_SetKeyframes(&p->animation, p->animationData, 0xE4, 0, 0);
                Anim_Step(&p->animation, p->animationData);
            } else
                Player_SetAnimation(p, ActorVariant_GetTableValue(0, 10), 64, 1);
            if ((p->actionEighteenDuration += 3) > 32)
                p->actionEighteenDuration = 32;
            p->resetValue6FC = 30;
            if (++p->tetherFrame > 19) {
                if (p->actionMode != 1) {
                    p->actionMode = 1;
                    p->statHundredths = 0;
                } else
                    p->actionMode = 0;
                Player_InitVisualResources(p);
                p->savedSpriteResource = 0xFFFF;
                if (p->unk536 != 0x15 && p->unk536 != 0x31)
                    p->active = 0;
                goto phaseFour;
            }
            break;
        case 3:
            if (!p->tetherFrame)
                Sound_PlaySong(0x192);
            Player_SetAnimation(p, ActorVariant_GetTableValue(p->actionMode, 10), 64, 1);
            p->resetValue6FC = 30;
            if (++p->tetherFrame > 15) {
            phaseFour:
                Player_SetActionPhase(p, 4);
            }
            break;
        case 4:
            p->resetValue6FC = 15;
            if (p->actionMode != 1) {
                if (Player_SetAnimation(p, ActorVariant_GetTableValue(p->actionMode, 11), 64, 1)) {
                    Player_UpdateFacingSide(p);
                    ActorSequence_Begin(p, 0, 0);
                    if (p->kind == 2) {
                        p->actionLatch464 = 1;
                        p->actionLatch465 = 1;
                        Player_UpdateFacingSide(p);
                        PlayerCue_EnterStandUpdate(p);
                        p->pendingAnimationTrigger = 1;
                    }
                }
            } else {
                if (!p->tetherFrame)
                    Anim_SetKeyframes(&p->animation, p->animationData, 0xE4, 1, 0);
                Anim_Step(&p->animation, p->animationData);
                if (++p->tetherFrame > 29) {
                    Player_UpdateFacingSide(p);
                    ActorSequence_Begin(p, 0, 0);
                    if (p->kind == 2) {
                        p->actionLatch464 = 1;
                        p->actionLatch465 = 1;
                        Player_UpdateFacingSide(p);
                        PlayerCue_EnterStandUpdate(p);
                        p->pendingAnimationTrigger = 1;
                    }
                }
            }
            break;
    }
}
