#include "global.h"
#include "player.h"
#include "contact_swarm.h"
#include "render.h"
#include "display_sequence_inline.h"
// confidence=low: action creates the spiral trail and advances its auxiliary sprite.
u32 Player_ConsumeStateEntry(struct Player *);
void Player_EmitQueuedEffects(struct Player *);
void Player_NotifyActionCategories(struct Player *);
void Sound_PlaySong(u32);
void SpiralTrail_CreateWithParticles(struct Player *, s32, s32, s32, s32);
void Player_AddControlFlags(struct Player *, s32);
s32 Player_SetAnimation(struct Player *, s32, s32, s32);
void ActorSequence_Begin(struct Player *, s32, s32);
static inline void addFlags(u32 *flags, u32 mask)
{
    *flags |= mask;
}
// confidence=low: provisional action interpretation from retained C and assembly.
void Player_UpdateSpiralTrailAction(struct Player *p)
{
    struct ContactSwarmSprite *sprite;
    struct DisplaySequenceState *state;
    const u16 *command;
    if (Player_ConsumeStateEntry(p) || !p->tetherFrame) {
        p->actionEighteenDuration = 0;
        Player_EmitQueuedEffects(p);
        Player_NotifyActionCategories(p);
        Sound_PlaySong(0x39A);
        SpiralTrail_CreateWithParticles(p, 1, 0, 32, 56);
    }
    addFlags((u32 *)((u8 *)p + 0x20), 0x100000);
    if ((p->tetherFrame & 7) >= 7 - (p->tetherFrame >> 2)) {
        Player_AddControlFlags(p, 0x41);
        p->previousPosition = p->position;
    }
    Player_SetAnimation(p, 0x9A, 64, 1);
    {
        u16 *resource = &p->spriteResource;
        u32 id = 0x16F;
        *resource = id;
        p->actionVariantResource = (u32)(gPaletteBankData + id * 32);
    }
    state = (void *)((u8 *)p + 0x3FC);
    Anim_SetSequence(state, *(void **)((u8 *)p + 0x410), 0, p->heading, p->side);
    sprite = (void *)((u8 *)p + 0x14C);
    command = state->commands + state->index;
    sprite->frame = *command >> 6;
    {
        union DisplaySequenceControl control;
        u32 bit, first;
        control.raw = state->flags;
        bit = 1;
        first = (u8)(control.raw & bit);
        if (first != (((*command & 0x30) >> 4) & bit))
            sprite->flags |= 4;
        else
            sprite->flags &= ~4;
    }
    {
        union DisplaySequenceControl control;
        u32 bit, first;
        control.raw = state->flags;
        bit = 2;
        first = (u8)(control.raw & bit);
        if (first != (((*command & 0x30) >> 4) & bit))
            sprite->flags |= 8;
        else
            sprite->flags &= ~8;
    }
    if (++state->elapsed >= state->duration) {
        state->elapsed = 0;
        if (state->flags & 4) {
            if (state->index == 0) {
                if (state->flags & 8)
                    state->elapsed = state->duration;
                else
                    state->index = state->frameCount - 1;
            } else {
                state->index--;
            }
        } else if (++state->index >= state->frameCount) {
            if ((u8)(state->flags & 8)) {
                state->elapsed = state->duration;
                state->index = state->frameCount - 1;
            } else {
                state->index = 0;
            }
        }
        command = state->commands + state->index;
        DisplaySequence_UpdateWeight(state, command);
        if (!state->duration)
            state->duration = 1;
    }
    p->actionEighteenParameter = 0x16F;
    if ((p->actionEighteenDuration += 3) > 32)
        p->actionEighteenDuration = 32;
    if (++p->tetherFrame > 31) {
        p->actionLatch464 = 1;
        p->actionLatch465 = 1;
        ActorSequence_Begin(p, 0, 0);
    }
}
