#include "contact_swarm.h"
#include "display_sequence_inline.h"
#include "input.h"
#include "motion_track.h"

s32 Vanargand_GetTargetLateralDistance(void *);

void Vanargand_SetPendingCue(void *, u16);
s32 Callback_InvokeActor(void *, void *);
void PairedSceneActor_ApplyToSelectedHalves(void *, s32, s32);
void SceneActors_StartUnmirroredAnimation(void *, void *, void *, s32, s32, s32);
void Debug_NudgeWorldPositionWithA(void *);
void MotionTrack_Update(void *);
void VanargandAfterimagePool_Spawn(void *, void *);
void Vanargand_StartActionSequence(void *);
void Vanargand_CreateTransientEffect(void *, u8);
void Vanargand_AdvanceEffectStageOne(void *);
void Vanargand_AdvanceEffectStageThree(void *);
void Sprite_ApplyAnimation(void *, void *);
void ScreenTransition_SendCommand(s32, s32, s32, s32, s32, s32, s32);
void PlayerContact_ApplySyntheticContact(void *, s32, s32, s32, s32, s32);
void Sound_PlaySong(u32);
s32 sub_081A6A08(void *);
extern const s16 gUnk_086149C4[];

struct VanargandSprite {
    struct ContactSwarmSprite sprite;
    u8 graphics[0x64 - 0x2C];
    struct DisplaySequenceState sequence;
    void *resource;
    u8 unknown78[8];
    struct ContactPosition offset;
};

struct VanargandAnimationParts {
    u8 animation[0x60];
    u8 paired[0xA4 - 0x60];
    u8 side, effectStage, unknownA6[2];
    struct VanargandSprite sprite;
};

struct VanargandSecondaryPart {
    u8 unknown0[0x48];
    struct DisplaySequenceState sequence;
    void *resource;
    u32 flags;
};
struct VanargandBoss {
    u8 unknown0[0x54 - 0x0];
    struct ContactPosition position;
    struct ContactPosition velocity;
    u8 unknown64[0x9C - 0x64];
    u32 flags;
    u8 unknownA0[0xA2 - 0xA0];
    u8 phase;
    u8 unknownA3[0xA4 - 0xA3];
    u32 timer;
    u8 unknownA8[0xAF - 0xA8];
    u8 finished;
    u8 unknownB0[0xB1 - 0xB0];
    u8 pending;
    u8 animationEnded;
    u8 unknownB3[0xCC - 0xB3];
    u8 heading;
    u8 unknownCD[0x104 - 0xCD];
    void *callback;
    u8 unknown108[0x10C - 0x108];
    void *contactActor;
    u8 unknown110[0x18C - 0x110];
    struct VanargandAnimationParts parts;
    u8 unknown2BC[0x3FC - 0x2BC];
    struct VanargandSecondaryPart secondary;
    u8 unknown45C[0x500 - 0x45C];
    u16 secondaryActive;
    u8 unknown502[0x534 - 0x502];
    struct MotionTrack track;
    void *pair;
    u8 unknown570[0x58E - 0x570];
    u16 contactForce;
    u8 unknown590[0xF18 - 0x590];
    u32 paletteState;
    u32 paletteFrame;
    u16 paletteStart;
    u16 paletteEnd;
    u32 variant;
    u8 unknownF28[0xF44 - 0xF28];
    void *afterimages;
};

static inline void Vg_SetFlags(struct VanargandBoss *actor, u32 mask)
{
    actor->flags |= mask;
}
static inline void Vg_ChangePhase(struct VanargandBoss *actor, u32 next)
{
    u32 zero = 0;
    actor->phase = next;
    actor->timer = zero;
}
static inline void Vg_SetPalette(struct VanargandBoss *actor, u32 first, u32 last, u32 frame)
{
    actor->paletteFrame = 0;
    actor->paletteStart = first;
    actor->paletteEnd = last;
    actor->paletteState = frame;
}
static inline void Vg_ResetSprite(struct VanargandSprite *sprite)
{
    u32 frame = 24;
    Sprite_ApplyAnimation(sprite, sprite->graphics);
    sprite->sprite.frame = frame;
}
static inline s32 Vg_ShiftTowardZero(s32 n, u32 shift)
{
    return n >= 0 ? n >> shift : -((-n) >> shift);
}
static inline void Vg_Polar(struct ContactPosition *out, s32 angle, s32 radius)
{
    out->x = Vg_ShiftTowardZero(gUnk_086149C4[(angle + 0x40) & 0xFF] * radius, 12);
    out->y = 0;
    out->z = Vg_ShiftTowardZero(gUnk_086149C4[angle] * radius, 12);
}
static inline u8 Vg_TrackEnded(struct VanargandBoss *actor)
{
    u8 ended = 0;
    if (actor->track.active == 2)
        ended = 1;
    return ended;
}
static inline void Vg_RestartSecondary(struct VanargandBoss *actor, u32 flags)
{
    struct VanargandSecondaryPart *part = &actor->secondary;
    Anim_SetSequence(&part->sequence, part->resource, 0, 0, flags);
    actor->secondaryActive = 1;
    part->flags = 0;
}
static inline void Vg_SetPosition(struct ContactPosition *out, s32 x, s32 y, s32 z)
{
    out->x = x;
    out->y = y;
    out->z = z;
}
static inline u32 Vg_081ABA64_StepFrame(struct VanargandSprite *owner)
{
    u32 done;
    struct DisplaySequenceState *state;
    const u16 *command;
    state = &owner->sequence;
    {
        u32 offset = DisplaySequence_GetIndex(state) << 1;
        const u16 *table;
        table = state->commands;
        command = (const u16 *)((u32)table + offset);
    }
    owner->sprite.frame = *command >> 6;
    {
        union DisplaySequenceControl control;
        u32 bit, first;
        control.raw = state->flags;
        bit = 1;
        first = (u8)(control.raw & bit);
        if (first != (((*command & 0x30) >> 4) & bit))
            owner->sprite.flags |= 4;
        else
            owner->sprite.flags &= ~4;
    }
    {
        union DisplaySequenceControl control;
        u32 bit, first;
        control.raw = state->flags;
        bit = 2;
        first = (u8)(control.raw & bit);
        if (first != (((*command & 0x30) >> 4) & bit))
            owner->sprite.flags |= 8;
        else
            owner->sprite.flags &= ~8;
    }
    if (++state->elapsed >= state->duration) {
        state->elapsed = 0;
        if (state->flags & 4) {
            if (state->index == 0) {
                if (state->flags & 8)
                    state->elapsed = state->duration;
                else
                    state->index = state->frameCount - 1;
                done = 1;
            } else {
                state->index--;
                done = 0;
            }
        } else {
            if (++state->index >= state->frameCount) {
                if (state->flags & 8) {
                    state->elapsed = state->duration;
                    state->index = state->frameCount - 1;
                } else {
                    state->index = 0;
                }
                done = 1;
            } else
                done = 0;
        }
        command = state->commands + state->index;
        state->frameWeight = DisplaySequence_MaskCommand(DisplaySequence_ReadCommand(command), 15);
        state->duration = (state->frameWeight * (s32)state->speed) >> 6;
        if (state->duration == 0)
            state->duration = 1;
    } else
        done = 0;
    return done;
}

// confidence=low: charged attack sequence with contact damage and a screen flash.
#define Vanargand_UpdateChargeSequence sub_081ABA64
void Vanargand_UpdateChargeSequence(struct VanargandBoss *arg0)
{
    struct VanargandBoss *self = arg0;
    struct VanargandAnimationParts *sub = &arg0->parts;
    u8 result;
    struct ContactPosition org, org2;
    struct ContactPosition projected1, projected2, projected3, projected4, projected5;
    s32 radius, heading;
    s32 angle;
    u32 sequenceEnded;
    u32 entryTimer;

    Vg_SetFlags(arg0, 8);
    Vanargand_SetPendingCue(arg0, 1);
    result = Callback_InvokeActor(arg0, arg0->callback);
    if (result != 0)
        return;

    if (arg0->pending != 0) {
        arg0->pending = result;
        PairedSceneActor_ApplyToSelectedHalves(self->pair, 3, 0x13);
        arg0->phase = 1;
        arg0->timer = result;
    }

    switch (arg0->phase) {
        case 0:
            Debug_NudgeWorldPositionWithA(&arg0->position);
            if ((gPadState[1] & 0x100) == 0)
                return;
            if (gPadState[0] & 1)
                sub->side = 1;
            else
                sub->side = 0;
            Vg_ChangePhase(arg0, 2);
            return;
        case 1:
            if (arg0->timer == 0) {
                SceneActors_StartUnmirroredAnimation(arg0, sub, sub->paired, 2, 1, 0);
                radius = Vanargand_GetTargetLateralDistance(arg0);
                if (radius > 0x200)
                    radius = 0x200;
                angle = arg0->heading;
                heading = angle - 0x20;
                angle = 0xE0;
                if ((u32)heading <= 0x7F)
                    angle = 0x60;
                Vg_Polar(&org, angle, radius);
                org.x += 0x380;
                org.y = 0x300;
                org.z += 0x380;
                MotionTrack_Initialize(&self->track, &arg0->position,
                                       (const struct MotionTrackVector *)&org, 0, 4);
                arg0->timer += 1;
                Vg_RestartSecondary(self, 0);
            }
            MotionTrack_Update(&self->track);
            if (Vg_TrackEnded(self)) {
                Vg_ChangePhase(arg0, 2);
                return;
            }
            VanargandAfterimagePool_Spawn(self->afterimages, self->parts.animation + 0x20);
            return;
        case 2:
            entryTimer = arg0->timer;
            if (entryTimer == 0) {
                SceneActors_StartUnmirroredAnimation(arg0, sub, sub->paired, 0xA, 2, 0);
                if (sub->side == 0) {
                    Vg_SetPalette(self, 2, 6, entryTimer);
                } else {
                    Vg_SetPalette(self, 3, 6, entryTimer);
                }
            }
            if (arg0->timer == 7) {
                Vg_ResetSprite(&sub->sprite);
                {
                    struct ContactPosition *dst = &sub->sprite.offset;
                    struct ContactPosition *point;
                    s32 a, b;
                    projected1.x = 0;
                    projected1.y = 0;
                    point = &projected1;
                    a = point->x;
                    b = point->y;
                    dst->x = Vg_ShiftTowardZero(((a + b) << 5) / 3 + 0x40, 1);
                    dst->y = 0x40;
                    dst->z = Vg_ShiftTowardZero(((-a + b) << 5) / 3 + 0x40, 1);
                }
            } else if (arg0->timer == 0xA) {
                {
                    struct ContactPosition *dst = &sub->sprite.offset;
                    struct ContactPosition *point;
                    s32 a, b;
                    projected2.x = 0;
                    projected2.y = 1;
                    point = &projected2;
                    a = point->x;
                    b = point->y;
                    dst->x = Vg_ShiftTowardZero(((a + b) << 5) / 3 + 0x40, 1);
                    dst->y = 0x40;
                    dst->z = Vg_ShiftTowardZero(((-a + b) << 5) / 3 + 0x40, 1);
                }
            }
            if (arg0->timer > 6)
                Vg_SetFlags(arg0, 0x80000);
            if (arg0->animationEnded != 0) {
                Vg_ChangePhase(arg0, 3);
                return;
            }
            arg0->timer += 1;
            return;
        case 3:
            Vg_SetFlags(arg0, 0x80000);
            if (arg0->timer == 0) {
                struct VanargandSprite *sprite = &sub->sprite;
                struct DisplaySequenceState *sequence = &sprite->sequence;
                Anim_SetSequence(sequence, sprite->resource, 3, 0, 0);
                arg0->timer += 1;
            }
            if (arg0->timer == 1) {
                sequenceEnded = Vg_081ABA64_StepFrame(&sub->sprite);
                if (sequenceEnded)
                    arg0->timer += 1;
                return;
            }
            arg0->timer += 1;
            if (arg0->timer <= 10)
                return;
            Vg_ChangePhase(arg0, 4);
            return;
        case 4:
            Vg_SetFlags(arg0, 0x80000);
            if (arg0->timer == 0) {
                struct VanargandSprite *sprite = &sub->sprite;
                struct DisplaySequenceState *sequence = &sprite->sequence;
                Anim_SetSequence(sequence, sprite->resource, 4, 0, 0);
                arg0->timer += 1;
                if (self->variant == 1)
                    Sound_PlaySong(0x543);
            }
            sequenceEnded = Vg_081ABA64_StepFrame(&sub->sprite);
            if (sequenceEnded && arg0->timer > 0xA) {
                Vg_ChangePhase(arg0, 5);
                return;
            }
            arg0->timer += 1;
            return;
        case 5:
            Vg_SetFlags(arg0, 0x80000);
            if (arg0->timer == 0) {
                SceneActors_StartUnmirroredAnimation(arg0, sub, sub->paired, 0xB, 2, 0);
                {
                    struct VanargandSprite *sprite = &sub->sprite;
                    struct DisplaySequenceState *sequence = &sprite->sequence;
                    Anim_SetSequence(sequence, sprite->resource, 5, 0, 0);
                }
                {
                    struct ContactPosition *dst = &sub->sprite.offset;
                    struct ContactPosition *point;
                    s32 a, b;
                    projected3.x = 0;
                    projected3.y = 3;
                    point = &projected3;
                    a = point->x;
                    b = point->y;
                    dst->x = Vg_ShiftTowardZero(((a + b) << 5) / 3 + 0x40, 1);
                    dst->y = 0x40;
                    dst->z = Vg_ShiftTowardZero(((-a + b) << 5) / 3 + 0x40, 1);
                }
                sequenceEnded = Vg_081ABA64_StepFrame(&sub->sprite);
                Vanargand_StartActionSequence((u8 *)self + 0x21C);
                Vanargand_CreateTransientEffect(&sub->sprite.sprite.position, sub->side);
                Sound_PlaySong(0x584);
            }
            arg0->timer += 1;
            if (arg0->timer <= 0x3C)
                return;
            Vg_ChangePhase(arg0, 6);
            if (self->parts.side == 0)
                ScreenTransition_SendCommand(0, 3, 0x1F, 0x1F, 0x1F, 0xFFFF, 0);
            else
                ScreenTransition_SendCommand(0, 3, 4, 4, 4, 0xFFFF, 0);
            return;
        case 6:
            Vg_SetFlags(arg0, 0x80000);
            if (arg0->timer == 0) {
                SceneActors_StartUnmirroredAnimation(arg0, sub, sub->paired, 0xB, 2, 0);
                Vanargand_AdvanceEffectStageOne((u8 *)self + 0x21C);
                {
                    s32 x = arg0->position.x;
                    s32 y = arg0->position.y;
                    s32 z = arg0->position.z;
                    struct MotionTrack *track;
                    s32 delta;
                    u32 mode;
                    track = &self->track;
                    delta = -0x100;
                    x += delta;
                    z += delta;
                    mode = 4;
                    Vg_SetPosition(&org2, x, y, z);
                    MotionTrack_Initialize(track, &arg0->position,
                                           (const struct MotionTrackVector *)&org2, 0, mode);
                }
            }
            sequenceEnded = Vg_081ABA64_StepFrame(&sub->sprite);
            if (sub_081A6A08(arg0)) {
                if (self->parts.side == 0)
                    PlayerContact_ApplySyntheticContact(arg0->contactActor, self->contactForce, 2,
                                                        0x20, 0, 1);
                else
                    PlayerContact_ApplySyntheticContact(arg0->contactActor, self->contactForce, 2,
                                                        0x20, 0, 2);
            }
            MotionTrack_Update(&self->track);
            if (self->parts.effectStage == 3) {
                Vg_ChangePhase(arg0, 7);
                return;
            }
            arg0->timer += 1;
            return;
        case 7:
            Vg_SetFlags(arg0, 0x80000);
            if (arg0->timer == 0)
                SceneActors_StartUnmirroredAnimation(arg0, sub, sub->paired, 0xB, 2, 0);
            sequenceEnded = Vg_081ABA64_StepFrame(&sub->sprite);
            if (arg0->timer <= 0x3B) {
                if (sub_081A6A08(arg0)) {
                    if (self->parts.side == 0)
                        PlayerContact_ApplySyntheticContact(arg0->contactActor, self->contactForce,
                                                            2, 0x20, 0, 1);
                    else
                        PlayerContact_ApplySyntheticContact(arg0->contactActor, self->contactForce,
                                                            2, 0x20, 0, 2);
                }
                if (arg0->timer & 1) {
                    Vg_SetPosition(&arg0->velocity, -2, 0, -2);
                }
            }
            if (arg0->timer == 0x3C)
                Vanargand_AdvanceEffectStageThree((u8 *)self + 0x21C);
            if (self->parts.effectStage == 0) {
                Vg_ChangePhase(arg0, 8);
                return;
            }
            arg0->timer += 1;
            return;
        case 8:
            Vg_SetFlags(arg0, 0x80000);
            if (arg0->timer == 0) {
                struct VanargandSprite *sprite = &sub->sprite;
                struct DisplaySequenceState *sequence = &sprite->sequence;
                Anim_SetSequence(sequence, sprite->resource, 6, 0, 0);
                arg0->timer += 1;
            }
            if (arg0->timer == 1) {
                sequenceEnded = Vg_081ABA64_StepFrame(&sub->sprite);
                if (sequenceEnded)
                    arg0->timer += 1;
                return;
            }
            Vg_SetPosition(&arg0->velocity, arg0->timer, 0, arg0->timer);
            arg0->timer += 1;
            if (arg0->timer <= 0xF)
                return;
            Vg_ChangePhase(arg0, 9);
            return;
        case 9:
            if (arg0->timer == 0) {
                SceneActors_StartUnmirroredAnimation(arg0, sub, sub->paired, 0xA, 2, 2);
                Vg_ResetSprite(&sub->sprite);
                {
                    struct ContactPosition *dst = &sub->sprite.offset;
                    struct ContactPosition *point;
                    s32 a, b;
                    projected4.x = 0;
                    projected4.y = 1;
                    point = &projected4;
                    a = point->x;
                    b = point->y;
                    dst->x = Vg_ShiftTowardZero(((a + b) << 5) / 3 + 0x40, 1);
                    dst->y = 0x40;
                    dst->z = Vg_ShiftTowardZero(((-a + b) << 5) / 3 + 0x40, 1);
                }
                if (sub->side == 0) {
                    Vg_SetPalette(self, 4, 5, 0);
                } else {
                    Vg_SetPalette(self, 5, 5, 0);
                }
                {
                    struct VanargandSecondaryPart *part = &self->secondary;
                    u32 zero = 0;
                    u32 flags = 4;
                    struct DisplaySequenceState *sequence = &part->sequence;
                    Anim_SetSequence(sequence, part->resource, 0, 0, flags);
                    self->secondaryActive = 1;
                    part->flags = zero;
                }
            }
            if (arg0->timer == 3) {
                struct ContactPosition *dst = &sub->sprite.offset;
                struct ContactPosition *point;
                s32 a, b;
                projected5.x = 0;
                projected5.y = 0;
                point = &projected5;
                a = point->x;
                b = point->y;
                dst->x = Vg_ShiftTowardZero(((a + b) << 5) / 3 + 0x40, 1);
                dst->y = 0x40;
                dst->z = Vg_ShiftTowardZero(((-a + b) << 5) / 3 + 0x40, 1);
            }
            if (arg0->timer <= 6)
                Vg_SetFlags(arg0, 0x80000);
            if (arg0->animationEnded != 0) {
                Vg_ChangePhase(arg0, 10);
                return;
            }
            arg0->timer += 1;
            return;
        case 10:
            arg0->finished = 1;
            Vg_ChangePhase(arg0, 0);
            return;
    }
}
