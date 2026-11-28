#include "global.h"
#include "bosses/vanargand/vanargand_pair_attack.h"
#include "scene/collision/contact_swarm.h"
#include "engine/video/display_sequence_inline.h"
#include "engine/input.h"
#include "scene/actor/motion_track.h"

s32 Vanargand_GetTargetLateralDistance(void *);

void Vanargand_SetPendingCue(void *, u16);
s32 Callback_InvokeActor(void *, void *);
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
s32 Vanargand_TestProjectedHorizontalRange(void *);
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

static inline void Vg_SetPalette(struct VanargandPairAttack *actor, u32 first, u32 last, u32 frame)
{
    actor->paletteSelector = 0;
    actor->paletteMode = first;
    actor->paletteShift = last;
    actor->paletteFrame = frame;
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
static inline void Vg_ProjectChargeOffset(struct ContactPosition *dst, s32 x, s32 y)
{
    dst->x = Vg_ShiftTowardZero(((x + y) * 32) / 3 + 0x40, 1);
    dst->y = 0x40;
    dst->z = Vg_ShiftTowardZero(((-x + y) * 32) / 3 + 0x40, 1);
}
static inline void Vg_SetChargeOffset(struct ContactPosition *dst, s32 x, s32 y)
{
    struct ContactPosition input;
    struct ContactPosition *point;
    s32 a, b;
    input.x = x;
    input.y = y;
    point = &input;
    a = point->x;
    b = point->y;
    Vg_ProjectChargeOffset(dst, a, b);
}

static inline void Vg_Polar(struct ContactPosition *out, s32 angle, s32 radius)
{
    out->x = Vg_ShiftTowardZero(gUnk_086149C4[(angle + 0x40) & 0xFF] * radius, 12);
    out->y = 0;
    out->z = Vg_ShiftTowardZero(gUnk_086149C4[angle] * radius, 12);
}
static inline u8 Vg_TrackEnded(struct VanargandPairAttack *actor)
{
    u8 ended = 0;
    if (actor->track.active == 2)
        ended = 1;
    return ended;
}
static inline void Vg_RestartSecondary(struct VanargandPairAttack *actor, u32 flags)
{
    struct VanargandDamagePart *part = &actor->part;
    Anim_SetSequence(&part->sequence, (void *)part->sequenceResource, 0, 0, flags);
    actor->part.animation = 1;
    part->sequenceId = 0;
}
static inline void Vg_SetPosition(struct SceneVec *out, s32 x, s32 y, s32 z)
{
    out->x = x;
    out->y = y;
    out->z = z;
}
static inline u32 Vg_StepSpriteFrame(struct VanargandSprite *owner)
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
void Vanargand_UpdateChargeSequence(struct VanargandPairAttack *p)
{
    struct VanargandPairAttack *self = p;
    struct VanargandAnimationParts *sub = (struct VanargandAnimationParts *)&p->animation;
    u8 result;
    struct ContactPosition org;
    struct SceneVec org2;
    s32 radius, heading;
    s32 angle;
    u32 sequenceEnded;

    VanargandPairAttack_SetFlags(p, 8);
    Vanargand_SetPendingCue(p, 1);
    result = Callback_InvokeActor(p, p->callback);
    if (result != 0)
        return;

    if (p->pending != 0) {
        p->pending = result;
        PairedSceneActor_ApplyToSelectedHalves(self->pair, 3, 0x13);
        p->phase = 1;
        p->timer = result;
    }

    switch (p->phase) {
        case 0:
            Debug_NudgeWorldPositionWithA(&p->position);
            if ((gPadState[1] & 0x100) == 0)
                return;
            if (gPadState[0] & 1)
                sub->side = 1;
            else
                sub->side = 0;
            VanargandPairAttack_EnterPhase(p, 2);
            return;
        case 1:
            if (p->timer == 0) {
                SceneActors_StartUnmirroredAnimation(p, sub, sub->paired, 2, 1, 0);
                radius = Vanargand_GetTargetLateralDistance(p);
                if (radius > 0x200)
                    radius = 0x200;
                angle = p->targetBearings[0];
                heading = angle - 0x20;
                angle = 0xE0;
                if ((u32)heading <= 0x7F)
                    angle = 0x60;
                Vg_Polar(&org, angle, radius);
                org.x += 0x380;
                org.y = 0x300;
                org.z += 0x380;
                MotionTrack_Initialize(&self->track, &p->position,
                                       (const struct MotionTrackVector *)&org, 0, 4);
                p->timer++;
                Vg_RestartSecondary(self, 0);
            }
            MotionTrack_Update(&self->track);
            if (Vg_TrackEnded(self)) {
                VanargandPairAttack_EnterPhase(p, 2);
                return;
            }
            VanargandAfterimagePool_Spawn(self->afterimages, &self->animation.position);
            return;
        case 2:
            if (p->timer == 0) {
                SceneActors_StartUnmirroredAnimation(p, sub, sub->paired, 0xA, 2, 0);
                if (sub->side == 0) {
                    Vg_SetPalette(self, 2, 6, 0);
                } else {
                    Vg_SetPalette(self, 3, 6, 0);
                }
            }
            if (p->timer == 7) {
                Vg_ResetSprite(&sub->sprite);
                Vg_SetChargeOffset(&sub->sprite.offset, 0, 0);
            } else if (p->timer == 0xA)
                Vg_SetChargeOffset(&sub->sprite.offset, 0, 1);
            if (p->timer > 6)
                VanargandPairAttack_SetFlags(p, 0x80000);
            if (p->animationEvent != 0) {
                VanargandPairAttack_EnterPhase(p, 3);
                return;
            }
            p->timer++;
            return;
        case 3:
            VanargandPairAttack_SetFlags(p, 0x80000);
            if (p->timer == 0) {
                struct VanargandSprite *sprite = &sub->sprite;
                Anim_SetSequence(&sprite->sequence, sprite->resource, 3, 0, 0);
                p->timer++;
            }
            if (p->timer == 1) {
                sequenceEnded = Vg_StepSpriteFrame(&sub->sprite);
                if (sequenceEnded)
                    p->timer++;
                return;
            }
            p->timer++;
            if (p->timer <= 10)
                return;
            VanargandPairAttack_EnterPhase(p, 4);
            return;
        case 4:
            VanargandPairAttack_SetFlags(p, 0x80000);
            if (p->timer == 0) {
                struct VanargandSprite *sprite = &sub->sprite;
                Anim_SetSequence(&sprite->sequence, sprite->resource, 4, 0, 0);
                p->timer++;
                if (self->variant == 1)
                    Sound_PlaySong(0x543);
            }
            sequenceEnded = Vg_StepSpriteFrame(&sub->sprite);
            if (sequenceEnded && p->timer > 0xA) {
                VanargandPairAttack_EnterPhase(p, 5);
                return;
            }
            p->timer++;
            return;
        case 5:
            VanargandPairAttack_SetFlags(p, 0x80000);
            if (p->timer == 0) {
                SceneActors_StartUnmirroredAnimation(p, sub, sub->paired, 0xB, 2, 0);
                {
                    struct VanargandSprite *sprite = &sub->sprite;
                    Anim_SetSequence(&sprite->sequence, sprite->resource, 5, 0, 0);
                }
                Vg_SetChargeOffset(&sub->sprite.offset, 0, 3);
                sequenceEnded = Vg_StepSpriteFrame(&sub->sprite);
                Vanargand_StartActionSequence(&self->actionSequence);
                Vanargand_CreateTransientEffect(&sub->sprite.sprite.position, sub->side);
                Sound_PlaySong(0x584);
            }
            p->timer++;
            if (p->timer <= 0x3C)
                return;
            VanargandPairAttack_EnterPhase(p, 6);
            if (self->actionSequence.playerReady == 0)
                ScreenTransition_SendCommand(0, 3, 0x1F, 0x1F, 0x1F, 0xFFFF, 0);
            else
                ScreenTransition_SendCommand(0, 3, 4, 4, 4, 0xFFFF, 0);
            return;
        case 6:
            VanargandPairAttack_SetFlags(p, 0x80000);
            if (p->timer == 0) {
                SceneActors_StartUnmirroredAnimation(p, sub, sub->paired, 0xB, 2, 0);
                Vanargand_AdvanceEffectStageOne(&self->actionSequence);
                {
                    s32 x = p->position.x;
                    s32 y = p->position.y;
                    s32 z = p->position.z;
                    struct MotionTrack *track;
                    u32 mode;
                    track = &self->track;
                    x += -0x100;
                    z += -0x100;
                    mode = 4;
                    Vg_SetPosition(&org2, x, y, z);
                    MotionTrack_Initialize(track, &p->position,
                                           (const struct MotionTrackVector *)&org2, 0, mode);
                }
            }
            sequenceEnded = Vg_StepSpriteFrame(&sub->sprite);
            if (Vanargand_TestProjectedHorizontalRange(p)) {
                if (self->actionSequence.playerReady == 0)
                    PlayerContact_ApplySyntheticContact(p->player, self->contactForce, 2, 0x20, 0,
                                                        1);
                else
                    PlayerContact_ApplySyntheticContact(p->player, self->contactForce, 2, 0x20, 0,
                                                        2);
            }
            MotionTrack_Update(&self->track);
            if (self->actionSequence.step == 3) {
                VanargandPairAttack_EnterPhase(p, 7);
                return;
            }
            p->timer++;
            return;
        case 7:
            VanargandPairAttack_SetFlags(p, 0x80000);
            if (p->timer == 0)
                SceneActors_StartUnmirroredAnimation(p, sub, sub->paired, 0xB, 2, 0);
            sequenceEnded = Vg_StepSpriteFrame(&sub->sprite);
            if (p->timer <= 0x3B) {
                if (Vanargand_TestProjectedHorizontalRange(p)) {
                    if (self->actionSequence.playerReady == 0)
                        PlayerContact_ApplySyntheticContact(p->player, self->contactForce, 2, 0x20,
                                                            0, 1);
                    else
                        PlayerContact_ApplySyntheticContact(p->player, self->contactForce, 2, 0x20,
                                                            0, 2);
                }
                if (p->timer & 1) {
                    Vg_SetPosition(&p->velocity, -2, 0, -2);
                }
            }
            if (p->timer == 0x3C)
                Vanargand_AdvanceEffectStageThree(&self->actionSequence);
            if (self->actionSequence.step == 0) {
                VanargandPairAttack_EnterPhase(p, 8);
                return;
            }
            p->timer++;
            return;
        case 8:
            VanargandPairAttack_SetFlags(p, 0x80000);
            if (p->timer == 0) {
                struct VanargandSprite *sprite = &sub->sprite;
                Anim_SetSequence(&sprite->sequence, sprite->resource, 6, 0, 0);
                p->timer++;
            }
            if (p->timer == 1) {
                sequenceEnded = Vg_StepSpriteFrame(&sub->sprite);
                if (sequenceEnded)
                    p->timer++;
                return;
            }
            Vg_SetPosition(&p->velocity, p->timer, 0, p->timer);
            p->timer++;
            if (p->timer <= 0xF)
                return;
            VanargandPairAttack_EnterPhase(p, 9);
            return;
        case 9:
            if (p->timer == 0) {
                SceneActors_StartUnmirroredAnimation(p, sub, sub->paired, 0xA, 2, 2);
                Vg_ResetSprite(&sub->sprite);
                Vg_SetChargeOffset(&sub->sprite.offset, 0, 1);
                if (sub->side == 0) {
                    Vg_SetPalette(self, 4, 5, 0);
                } else {
                    Vg_SetPalette(self, 5, 5, 0);
                }
                {
                    struct VanargandDamagePart *part = &self->part;
                    u32 zero = 0;
                    u32 flags = 4;
                    Anim_SetSequence(&part->sequence, (void *)part->sequenceResource, 0, 0, flags);
                    self->part.animation = 1;
                    part->sequenceId = zero;
                }
            }
            if (p->timer == 3)
                Vg_SetChargeOffset(&sub->sprite.offset, 0, 0);
            if (p->timer <= 6)
                VanargandPairAttack_SetFlags(p, 0x80000);
            if (p->animationEvent != 0) {
                VanargandPairAttack_EnterPhase(p, 10);
                return;
            }
            p->timer++;
            return;
        case 10:
            p->finished = 1;
            VanargandPairAttack_EnterPhase(p, 0);
            return;
    }
}
