// MATCHING_CONCESSION: register-pinning
#include "contact_swarm.h"
#include "player.h"
#include "engine.h"
#include "ram.h"
#include "player_cue_sequence.h"
// confidence=low: motion eleven toggles actionMode two with a secondary sprite animation.
u32 Player_ConsumeStateEntry(struct Player *);
void Player_SetPendingCallback(struct Player *, void (*)(struct Player *));
void Player_OnMotion11Exit(struct Player *);
void Player_AddControlFlags(struct Player *, s32);
void Player_SetActionPhase(struct Player *, u8);
s32 ActorVariant_GetTableValue(u32, s32);
s32 Player_SetAnimation(struct Player *, s32, s32, s32);
void ActorAttachment_Create(struct Player *, s32, struct SceneVec *, s32, s32, s32);
void Player_EmitQueuedEffects(struct Player *);
void Player_NotifyActionCategories(struct Player *);
void Player_InitVisualResources(struct Player *);
void Player_UpdateFacingSide(struct Player *);
void ActorSequence_Begin(struct Player *, s32, s32);
void PlayerCue_EnterStandUpdate(struct Player *);
void PlayerCue_AcknowledgePendingTrigger(struct Player *);
void ScreenTransition_SendCommand(s32, s32, s32, s32, s32, s32, s32);
#define H(off)  (*(u16 *)((u8 *)p + (off)))
#define SH(off) (*(s16 *)((u8 *)p + (off)))
#define B(off)  (*((u8 *)p + (off)))
#define W(off)  (*(u32 *)((u8 *)p + (off)))
#define SPRITE  ((struct ContactSwarmSprite *)((u8 *)p + 0x1D0))
#define SEQ     ((struct DisplaySequenceState *)((u8 *)p + 0x218))
struct AlternateSpriteView {
    u8 pad[0x1D0];
    struct ContactSwarmSprite sprite;
    u8 pad200[0x218 - 0x1D0 - sizeof(struct ContactSwarmSprite)];
    struct DisplaySequenceState sequence;
    u32 done;
    const void *resource;
};
#define V ((struct AlternateSpriteView *)p)
void Player_UpdateAlternateFormCue(struct Player *p)
{
    if (Player_ConsumeStateEntry(p)) {
        if (p->actionMode != 2) {
            if (p->statHundredths > 999 || (gEngineFlags & 0x1000) || p->kind == 2)
                p->statusValue = 4000;
            V->sprite.position.y = 450 + *(u16 *)((u8 *)gGameState + 0x32);
            V->sprite.position.pad = 1;
            B(0x432) = 2;
            B(0x433) = 1;
        } else {
            V->sprite.position.y = *(u16 *)((u8 *)gGameState + 0x32) + 220;
            V->sprite.position.pad = 0;
            B(0x432) = 0;
            B(0x433) = 1;
        }
        if (p->statHundredths > 999 && !(gEngineFlags & 0x1000) && p->kind != 2) {
            p->actionEighteenParameter = 0x16E;
            p->actionEighteenDuration = 0;
            p->actionEighteenEnabled = 1;
        } else
            p->actionEighteenEnabled = 0;
        Sound_PlaySong(0x306);
        if (p->statHundredths <= 999 && !(gEngineFlags & 0x1000) && p->kind != 2)
            W(0x1D0) |= 1;
        else {
            W(0x1D0) &= ~1;
            Anim_SetSequence(&V->sequence, V->resource, 21, 0, 0);
            V->done = 0;
        }
        Player_SetPendingCallback(p, Player_OnMotion11Exit);
        p->actionCountdown = 0;
        H(0x4E6) = 0;
        p->terrainRecoveryCounter = 0;
        H(0x4EA) = 0;
    }
    if (gEngineFlags & 0x1000)
        Player_AddControlFlags(p, 8);
    {
        u32 mask = 0x100000;
        p->statusFlags = mask | p->statusFlags;
    }
    p->statusValue = 4000;
    if (V->done)
        Anim_SetSequence(&V->sequence, V->resource, 21, 0, 0);
    V->done = PlayerCue_StepSequenceEnded(&V->sprite, &V->sequence);
    if (H(0x222) == 21) {
        if (V->sprite.position.pad == 1)
            V->sprite.position.y -= 10;
        else
            V->sprite.position.y += 10;
    }
    {
        u8 *game;
        if (V->sprite.position.pad == 1) {
            s16 *height = &V->sprite.position.y;
            register u8 *volatile *address asm("r2");
            u8 *base;
            s32 y = *height;
            address = (u8 *volatile *)&gGameState;
            base = *address;
            if (y < *(s16 *)(base + 0x32) + 220)
                *height = *(u16 *)(base + 0x32) + 220;
            game = *address;
        } else
            game = *(u8 *volatile *)&gGameState;
        V->sprite.position.x = *(u16 *)(game + 0x30) - 6;
        V->sprite.position.z = *(u16 *)(game + 0x34) + 6;
    }
    switch (p->tetherPhase) {
        case 0: {
            struct SceneVec v;
            p->facing = 4;
            p->heading = 4;
            p->side = 0;
            Player_SetAnimation(p, ActorVariant_GetTableValue(p->actionMode, 0), 64, 1);
            p->heading >>= 2;
            v.x = 0;
            v.y = 0;
            v.z = 15;
            if (p->actionEighteenEnabled || p->kind == 2)
                ActorAttachment_Create(p, 0, &v, 50, 50, 1);
            else
                ActorAttachment_Create(p, 0, &v, 50, 50, 0);
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
                if (p->statHundredths > 999 || (gEngineFlags & 0x1000) || p->kind == 2 ||
                    p->actionMode == 2) {
                    if (p->actionMode != 2) {
                        Sound_PlaySong(0x4F3);
                        Player_EmitQueuedEffects(p);
                        Player_NotifyActionCategories(p);
                    }
                    Player_SetActionPhase(p, 2);
                } else
                    Player_SetActionPhase(p, 3);
            }
            break;
        case 2:
            if ((p->tetherFrame & 7) < 7 - (p->tetherFrame >> 2))
                Player_SetAnimation(p, ActorVariant_GetTableValue(p->actionMode, 10), 64, 1);
            else if (p->actionMode != 2)
                Player_SetAnimation(p, ActorVariant_GetTableValue(2, 10), 64, 1);
            else
                Player_SetAnimation(p, ActorVariant_GetTableValue(0, 10), 64, 1);
            if ((p->actionEighteenDuration += 3) > 32)
                p->actionEighteenDuration = 32;
            p->resetValue6FC = 30;
            if (++p->tetherFrame > 9) {
                if (p->actionMode != 2) {
                    p->actionMode = 2;
                    {
                        u32 one = 1;
                        u32 duration = 1800;
                        gPlayerActionPending = one;
                        gPlayerCueTimer = duration;
                    }
                    ScreenTransition_SendCommand(0, 4, 31, 31, 20, 0x1FFF, 0);
                    gPlayerEffectPendingCount++;
                    p->statHundredths = 0;
                } else
                    p->actionMode = 0;
                Player_InitVisualResources(p);
                p->savedSpriteResource = 0xFFFF;
                if (p->unk536 != 0x15 && p->unk536 != 0x31)
                    p->active = 0;
                Player_SetActionPhase(p, 4);
                W(0x1D0) |= 1;
            }
            break;
        case 3:
            if (!p->tetherFrame)
                Sound_PlaySong(0x192);
            Player_SetAnimation(p, ActorVariant_GetTableValue(p->actionMode, 10), 64, 1);
            p->resetValue6FC = 30;
            if (++p->tetherFrame > 15)
                Player_SetActionPhase(p, 4);
            gPlayerEffectPendingCount = 0;
            break;
        case 4:
            p->resetValue6FC = 15;
            p->tetherFrame++;
            if (p->actionMode == 2) {
                H(0x4E6) = 6;
                H(0x4EA) = 18;
                Anim_SetKeyframes(&p->animation, p->animationData, 0x1AB, 1, 0);
                Anim_Step(&p->animation, p->animationData);
            } else {
                if (Player_SetAnimation(p, ActorVariant_GetTableValue(p->actionMode, 11), 64, 1)) {
                    Player_UpdateFacingSide(p);
                    if (p->kind != 2) {
                        ActorSequence_Begin(p, 0, 0);
                    } else {
                        p->actionLatch464 = 1;
                        if (p->actionMode == 2)
                            p->actionLatch465 = 2;
                        else
                            p->actionLatch465 = 0;
                        Player_UpdateFacingSide(p);
                        PlayerCue_EnterStandUpdate(p);
                        p->pendingAnimationTrigger = 1;
                    }
                    break;
                }
            }
            gPlayerEffectPendingCount = 0;
            Player_InitVisualResources(p);
            p->savedSpriteResource = 0xFFFF;
            p->actionEighteenDuration = 0;
            if (++p->tetherFrame > 29) {
                Player_UpdateFacingSide(p);
                if (p->kind != 2)
                    ActorSequence_Begin(p, 0, 0);
                else {
                    p->actionLatch464 = 1;
                    if (p->actionMode == 2)
                        p->actionLatch465 = 2;
                    else
                        p->actionLatch465 = 0;
                    Player_UpdateFacingSide(p);
                    PlayerCue_EnterStandUpdate(p);
                    p->pendingAnimationTrigger = 1;
                }
                if (p->kindTwoState == 2)
                    PlayerCue_AcknowledgePendingTrigger(p);
            }
            break;
    }
}
