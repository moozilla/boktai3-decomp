// MATCHING_CONCESSION: register-pinning
#include "contact_swarm.h"
#include "player.h"
#include "engine.h"
#include "ram.h"
static inline u32 Step(struct ContactSwarmSprite *sp, struct DisplaySequenceState *st)
{
    struct ContactSwarmSprite *sprite = sp;
    struct DisplaySequenceState *state = st;
    const volatile u16 *command;
    u32 duration, flags, bit;
    u32 done;
    {
        u32 offset = state->index << 1;
        const volatile u16 *table;
        table = state->commands;
        command = (const volatile u16 *)((u32)table + offset);
    }
    sprite->frame = *command >> 6;
    {
        u32 input = state->flags;
        register u32 first asm("r1");
        u32 maskBit = 1;
        bit = maskBit;
        asm("" : "+r"(bit));
        first = bit;
        first &= input;
        if (first != (((*command & 0x30) >> 4) & bit))
            sprite->flags |= 4;
        else
            sprite->flags &= ~4;
    }
    {
        u32 input = state->flags;
        u32 bit;
        register u32 first asm("r1");
        bit = 2;
        asm("" : "+r"(bit));
        first = bit;
        first &= input;
        asm("" : "+r"(first));
        first = (u8)first;
        if (first != (((*command & 0x30) >> 4) & bit))
            sprite->flags |= 8;
        else
            sprite->flags &= ~8;
    }
    {
        u32 elapsed = state->elapsed + 1;
        register u32 mask asm("r5");
        register u32 temp asm("r2");
        register u32 savedDuration asm("r3");
        state->elapsed = elapsed;
        temp = 0xFFFF;
        asm("" : "+r"(temp));
        mask = temp;
        duration = state->duration;
        elapsed = (u16)elapsed;
        savedDuration = duration;
        if (elapsed >= duration) {
            state->elapsed = 0;
            flags = state->flags;
            if (flags & 4) {
                u32 index = state->index;
                if (index == 0) {
                    u32 hold = 8;
                    hold &= flags;
                    if (hold) {
                        state->elapsed = savedDuration;
                    } else {
                        state->index = state->frameCount - 1;
                    }
                    done = 1;
                } else {
                    state->index = index - 1;
                    done = 0;
                }
            } else {
                u32 index = state->index + 1;
                u32 frameCount;
                u32 endFlag;
                state->index = index;
                frameCount = state->frameCount;
                if ((index & mask) >= frameCount) {
                    endFlag = flags & 8;
                    asm("" : "+r"(endFlag));
                    endFlag = (u8)endFlag;
                    if (endFlag) {
                        state->elapsed = duration;
                        {
                            u32 lastIndex = frameCount - 1;
                            state->index = lastIndex;
                        }
                    } else {
                        state->index = 0;
                    }
                    done = 1;
                } else {
                    done = 0;
                }
            }
            {
                u32 offset = state->index << 1;
                const volatile u16 *table;
                table = state->commands;
                command = (const volatile u16 *)((u32)table + offset);
            }
            state->frameWeight = *command & 15;
            {
                register s32 weight asm("r1") = state->frameWeight;
                s32 speed;
                speed = state->speed;
                speed = (speed * weight) >> 6;
                state->duration = speed;
                if (!(u8)speed)
                    state->duration = 1;
            }
        } else
            done = 0;
    }
    return done;
}

// confidence=low: motion eleven toggles actionMode two with a secondary sprite animation.
extern u16 gUnk_02000528, gUnk_02000590;
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
    V->done = Step(&V->sprite, &V->sequence);
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
                        gUnk_02000528 = one;
                        gPlayerCueTimer = duration;
                    }
                    ScreenTransition_SendCommand(0, 4, 31, 31, 20, 0x1FFF, 0);
                    gUnk_02000590++;
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
            gUnk_02000590 = 0;
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
            gUnk_02000590 = 0;
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
