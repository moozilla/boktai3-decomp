#include "scene_actor_chase.h"
#include "scene_particle_burst_api.h"
#include "global.h"
#include "scene_actor_drawable_flags.h"
#include "scene_actor_chase_shared.h"
#include "player.h"
#include "scene_actor_control_phase.h"
#include "rotation_phase.h"
#include "scene_actor_reset.h"
#include "axe.h"
#include "math_shift.h"
#include "word_flags.h"
#include "scene_actor_animation.h"
#include "scene_actor_state.h"
#include "scene_actor_behavior.h"
#include "scene_actor_reaction.h"
#include "script_utility.h"
#include "engine_globals.h"
#include "ram.h"
#include "scene_actor_collision.h"
#include "contact_actor.h"
#include "scene_actor_terrain.h"
#include "engine.h"
#include "display_track.h"
#include "render.h"
#include "scene_actor_palette.h"
#include "scene_actor_placement.h"
#include "scene_actor_registry.h"
#include "player_cue_sequence.h"
#include "display_animation.h"
#include "contact_swarm.h"
#include "display_sprite_playback.h"
#include "display_sequence_inline.h"
#include "scene_route_sample.h"
#include "scene_actor_init_group.h"
#include "scene_actor_init_primitives.h"
#include "scene_actor_init_graphics.h"
#include "scene_actor_init_body.h"
#include "scene_actor_turn.h"
#include "movement_curve.h"
#include "scene_actor_displacement.h"
#include "scene_actor_terrain_reaction.h"
#include "vanargand_parts.h"

struct AxeTrailSprite {
    u32 flags;
    u8 pad4[2];
    u8 field6;
};

void Axe_EmitMotionTrail(struct ChaseActor *actor);

struct AxeActor;

void Axe_UpdateReturningTrail(struct AxeActor *p, s32 frame);

void Axe_UpdateTrailFadeIn(struct ChaseActor *actor, s32 frame)
{
    if (SceneActorReaction_TakePrepared(actor)) {
        struct AxeTrailSprite *sprite = actor->base.drawable;
        sprite->flags |= 2;
    }
    Axe_EmitMotionTrail(actor);
    {
        struct AxeTrailSprite *sprite = actor->base.drawable;
        if (sprite->field6 <= 0x13)
            sprite->field6++;
    }
    if (frame > 0x3C) {
        struct AxeTrailSprite *sprite = actor->base.drawable;
        sprite->field6 = 0x14;
        Chase_SetCtl(actor, Axe_UpdateReturningTrail, 0x28);
    }
}

// confidence=low: axe attack behavior inferred from orbit and trail state updates.

// Two instructions differ: the global address load is scheduled after the stack-vector address.

// confidence=low: sends the axe trail toward the player, then returns it to the actor.

struct AxeTrail {
    u8 unknown0[0xA24];
    struct ContactPosition position, velocity;
    u8 angle, heading, spinRate;
    u8 unknownA37[0x405];
    u8 angleDecay, speed;
};

struct AxeActor {
    u8 unknown0[4];
    u8 *sprite;
    u8 unknown8[0x54];
    struct ContactPosition position;
    u8 unknown64[0x85];
    u8 facing;
    u8 unknownEA[0x1A];
    u32 word104;
    u8 unknown108[0xC];
    u32 flags114;
    u8 unknown118[0x104];
    void *callback;
    u8 unknown220[0x90];
    u8 state, unknown2B1, entered, unknown2B3[2], phase;
    u8 unknown2B6[0x12];
    u16 timer;
    u8 unknown2CA[0x106];
    struct AxeTrail *trail;
};

extern const s16 gUnk_086149C4[];

void Axe_UpdateTrailColliders(u8 *actor);

void ActorPhase_EmitTrailSprite(struct ChaseActor *actor, const struct ContactPosition *position,
                                u32 angle);

void Sound_PlaySong(u32);

s32 Math_Atan2(s32, s32);

void SceneActorControl_SettleRotation(struct RoomActor *actor, s32 elapsed);

static inline s32 Scale(s32 value)
{
    if (value >= 0)
        return value >> 12;
    return -(-value >> 12);
}

static inline void Velocity(struct ContactPosition *out, u8 angle, s32 speed)
{
    out->x = Scale(speed * gUnk_086149C4[(angle + 64) & 255]);
    out->y = 0;
    out->z = Scale(speed * gUnk_086149C4[angle]);
}

static inline void Advance(struct ContactPosition *a, struct ContactPosition *b)
{
    a->x = b->x + a->x;
    a->y = b->y + a->y;
    a->z = b->z + a->z;
}

static inline s32 AngleDifference(s32 a, s32 b)
{
    s32 delta = a - b;
    if (delta > 128)
        delta -= 256;
    else if (delta < -128)
        delta += 256;
    return delta;
}

static inline s32 LengthSquared(struct ContactPosition *v)
{
    return v->x * v->x + v->z * v->z;
}

static inline s32 Absolute(s32 value)
{
    if (value < 0)
        value = -value;
    return value;
}

void Axe_UpdateReturningTrail(struct AxeActor *p, s32 frame)
{
    struct AxeTrail *trail = p->trail;
    u32 previous;
    if (SceneActorControl_TakeFlag(&p->entered)) {
        *(u32 *)p->sprite |= 2;
        Velocity(&trail->velocity, p->facing, 12);
        trail->heading = p->facing;
        trail->position = p->position;
        trail->angle = p->sprite[6];
        trail->spinRate = 1;
    }
    previous = p->sprite[6];
    p->sprite[6] = previous - trail->spinRate;
    if ((frame & 1) && trail->spinRate <= 11)
        trail->spinRate++;
    Axe_UpdateTrailColliders(p);
    if ((s32)previous < p->sprite[6])
        Sound_PlaySong(0x169);
    if (frame <= 119) {
        struct ContactPosition delta;
        s32 difference;
        ActorPhase_EmitTrailSprite(p, &trail->position, trail->angle);
        Advance(&trail->position, &trail->velocity);
        trail->angle -= trail->angleDecay;
        {
            u8 **gp = &gPlayers;
            struct ContactPosition *d;
            d = &delta;
            Vanargand_Difference(d, (void *)(*gp + 0x30), &trail->position);
        }
        difference = AngleDifference(Math_Atan2(delta.x, delta.z), trail->heading);
        if (Absolute(difference) > 32)
            trail->heading += 4;
        Velocity(&trail->velocity, trail->heading, trail->speed);
    } else {
        struct ContactPosition delta;
        s32 difference;
        ActorPhase_EmitTrailSprite(p, &trail->position, trail->angle);
        Advance(&trail->position, &trail->velocity);
        trail->angle -= 6;
        Vanargand_Difference(&delta, &p->position, &trail->position);
        difference = AngleDifference(Math_Atan2(delta.x, delta.z), trail->heading);
        if (Absolute(difference) > 12)
            trail->heading += 6;
        Velocity(&trail->velocity, trail->heading, 8);
        if (LengthSquared(&delta) <= 1023 || frame > 600) {
            trail->position = p->position;
            SceneActorChase_SetControl(p, SceneActorControl_SettleRotation, 0x29);
        }
    }
}

void SceneActorControl_SettleRotation(struct RoomActor *actor, s32 elapsed)
{
    u8 *state = (u8 *)actor->context;
    struct RotatingDrawable *drawable = actor->drawable;
    s32 angle;
    u32 previous;
    if (SceneActorControl_TakeFlag(&actor->field2B2)) {
        drawable->flags |= 2;
        state[0xA36] = 12;
        state[0xEAF] = 30;
    }
    angle = NormalizeAngle(drawable->angle);
    Axe_EmitMotionTrail(actor);
    previous = drawable->angle;
    drawable->angle = previous - state[0xA36];
    if ((s32)previous < ((struct RotatingDrawable *)actor->drawable)->angle)
        Sound_PlaySong(0x169);
    if (elapsed > 29) {
        s32 magnitude = angle;
        if (magnitude < 0)
            magnitude = -magnitude;
        if (magnitude <= 12) {
            drawable->angle = 0;
            actor->stepDone = 1;
        }
    }
}

struct AxeFadeSprite {
    u32 flags;
    u8 pad4[2];
    u8 field6;
};

void Axe_UpdateFadeOut(struct ChaseActor *actor, s32 frame)
{
    u8 entered = SceneActorReaction_TakePrepared(actor);
    struct AxeFadeSprite *sprite = actor->base.drawable;
    s32 previous;
    if (entered)
        sprite->flags |= 2;
    previous = sprite->field6;
    sprite->field6 = previous - 6;
    if (previous < ((struct AxeFadeSprite *)actor->base.drawable)->field6)
        Sound_PlaySong(0x169);
    if (frame > 0x2B) {
        ((struct AxeFadeSprite *)actor->base.drawable)->field6 = 0;
        actor->base.stepDone = 1;
    }
}

void SceneActorControl_RecoverRotation(struct ChaseActor *actor, s32 elapsed);

static inline s32 FoldAngle(s32 a)
{
    if (a > 128)
        return a - 256;
    if (a < -128)
        return a + 256;
    return a;
}

static inline s32 Magnitude(s32 a)
{
    s32 result = a;
    if (result < 0)
        result = -result;
    return result;
}

void SceneActorControl_TiltRotation(struct ChaseActor *actor, s32 elapsed)
{
    s32 angle_2;
    u8 *state = actor->base.context;
    struct RotatingDrawable *drawable = actor->base.drawable;
    s32 angle, magnitude;
    if (SceneActorControl_TakeFlag(&actor->base.field2B2)) {
        u8 saved;
        drawable->flags |= 2;
        saved = actor->base.tracking.heading;
        state[0xED3] = saved;
    }
    angle = FoldAngle(drawable->angle);
    magnitude = angle;
    if (magnitude < 0) {
        magnitude = -magnitude;
    }
    if (magnitude > 48) {
        if (elapsed > 42) {
            RotationPhase_Change(actor, SceneActorControl_RecoverRotation, 50);
        }
    } else {
        angle_2 = FoldAngle(32 - state[0xED3]);
        if (angle_2 < 0) {
            drawable->angle -= 2;
        } else {
            drawable->angle += 2;
        }
    }
}

s32 Mod(s32, s32);

void Axe_SpawnHeadingBurst(struct ChaseActor *actor, s32 count);

void SceneActorControl_FinishRotation(struct ChaseActor *actor, s32 elapsed);

static inline s32 FoldRotation(s32 a)
{
    if (a > 128)
        return a - 256;
    if (a < -128)
        return a + 256;
    return a;
}

void SceneActorControl_RecoverRotation(struct ChaseActor *actor, s32 elapsed)
{
    struct Axe *state = actor->base.context;
    struct RotatingDrawable *drawable = actor->base.drawable;
    s32 angle;
    if (SceneActorControl_TakeFlag(&actor->base.field2B2)) {
        drawable->flags |= 2;
        state->acceleration = 4;
        Sound_PlaySong(0x1E5);
    }
    angle = NormalizeAngle(32 - state->savedAngle);
    if (angle < 0) {
        u32 step = state->acceleration;
        {
            u32 cur = drawable->angle;
            step = step + cur;
            drawable->angle = step;
        }
    } else
        drawable->angle -= state->acceleration;
    if (Mod(elapsed, 4) == 0)
        state->acceleration += 4;
    angle = NormalizeAngle(drawable->angle);
    Axe_EmitMotionTrail(actor);
    {
        s32 magnitude = angle;
        if (magnitude < 0)
            magnitude = -magnitude;
        if (magnitude > 96) {
            RotationPhase_Change(actor, SceneActorControl_FinishRotation, 51);
            state->flagsE4C = RotationPhase_Mask(&state->flagsE4C, ~0x200);
            state->parameterEB2 = 32;
            Axe_SpawnHeadingBurst(actor, 8);
        }
    }
}

void SceneActorControl_ResetRotation(struct ChaseActor *actor);

static inline u16 ClearRotationFlag(u16 *flags, u32 mask)
{
    return mask & *flags;
}

void SceneActorControl_FinishRotation(struct ChaseActor *actor, s32 elapsed)
{
    u8 *state = actor->base.context;
    struct RotatingDrawable *drawable = actor->base.drawable;
    if (SceneActorControl_TakeFlag(&actor->base.field2B2)) {
        drawable->flags |= 2;
        state[0xEB1] = 40;
        Sound_PlaySong(0x1E3);
        actor->base.placementFlags = ClearRotationFlag(&actor->base.placementFlags, ~0x1000);
    }
    if (Mod(elapsed, 6) == 0) {
        u32 mask = 32;
        u16 *flags = &actor->base.reactionFlags;
        mask |= *flags;
        *flags = mask;
    }
    if (elapsed > 80)
        RotationPhase_Change(actor, SceneActorControl_ResetRotation, 52);
}

// confidence=low: saves the player position and turns the axe before the attack callback.

void Axe_UpdateTargetLunge(struct ChaseActor *a, s32 frame);

void Axe_UpdateTargetWindup(struct ChaseActor *actor, s32 elapsed)
{
    struct Axe *state = actor->base.context;
    struct RotatingDrawable *drawable = actor->base.drawable;
    s32 angle, magnitude;
    if (SceneActorControl_TakeFlag(&actor->base.field2B2)) {
        drawable->flags |= 2;
        state->savedAngle = actor->base.tracking.heading;
        state->target = *(struct ContactPosition *)&gPlayers[0]->position;
        state->active = 1;
        Sound_PlaySong(0x16F);
    }
    angle = NormalizeAngle(drawable->angle);
    magnitude = angle;
    if (magnitude < 0)
        magnitude = -magnitude;
    if (magnitude > 48) {
        s32 delay;
        if (actor->base.health <= state->healthThreshold)
            delay = state->delay >> 1;
        else
            delay = state->delay;
        if (elapsed > delay) {
            SceneActorChase_SetControl(actor, Axe_UpdateTargetLunge, 0x36);
        }
    } else {
        s32 step, turn;
        if (actor->base.health <= state->healthThreshold)
            step = state->turnStep;
        else
            step = state->turnStep >> 1;
        if (step <= 0)
            step = 1;
        turn = NormalizeAngle(32 - state->savedAngle);
        if (turn < 0)
            drawable->angle -= step;
        else
            drawable->angle += step;
    }
}

// confidence=low: launches toward the player and brakes sprite rotation on arrival.

s32 Div(s32, s32);

struct S080FB958;

void Axe_SetAuxiliarySpriteRenderState(struct ChaseActor *actor, const void *palette, u32 index);

static inline s32 Axe_ShiftSigned(s32 v)
{
    return v >= 0 ? v >> 6 : -(-v >> 6);
}

struct AxeLungePlayer {
    u8 unknown0[0x30];
    struct ContactPosition position;
};

static inline void Axe_DeltaFromPlayer(u8 **player, struct ContactPosition *v,
                                       struct ContactPosition *b)
{
    struct ContactPosition *a = &((struct AxeLungePlayer *)*player)->position;
    v->x = a->x - b->x;
    v->y = a->y - b->y;
    v->z = a->z - b->z;
}

struct AxeLungeState {
    u8 unknown0[0x8A8];
    u8 auxiliary[0xE48 - 0x8A8];
    s32 healthThreshold;
    u32 flags;
    u8 unknownE50[0xEAE - 0xE50];
    u8 angularSpeed;
    u8 unknownEAF[3];
    u8 trailDuration;
    u8 delay;
    u8 unknownEB4[0xED1 - 0xEB4];
    u8 queued;
    u8 unknownED2;
    u8 angle;
};

void Axe_UpdateTargetLunge(struct ChaseActor *a, s32 frame)
{
    struct AxeLungeState *state = a->base.context;
    struct RotatingDrawable *sprite = a->base.drawable;
    u8 entered = SceneActorControl_TakeFlag(&a->base.field2B2);
    s32 angle;
    u32 *flags = &a->base.statusFlags;
    if (entered) {
        struct ContactPosition delta;
        sprite->flags |= 2;
        state->angularSpeed = 2;
        Axe_DeltaFromPlayer(&gPlayers, &delta, &a->base.body.position);
        if (a->base.health <= (state->healthThreshold)) {
            a->base.secondaryMotion.x = Div(delta.x, 44);
            a->base.secondaryMotion.y = 44;
            a->base.secondaryMotion.z = Div(delta.z, 44);
        } else {
            a->base.secondaryMotion.x = Axe_ShiftSigned(delta.x);
            a->base.secondaryMotion.y = 64;
            a->base.secondaryMotion.z = Axe_ShiftSigned(delta.z);
        }
        state->delay = 0;
        {
            u32 *entryFlags = &a->base.statusFlags;
            u32 mask = ~0x100000;
            *entryFlags &= mask;
            Axe_SetAuxiliarySpriteRenderState(a, state->auxiliary, 0x13D);
            Sound_PlaySong(0x1E5);
            flags = entryFlags;
        }
    }
    a->base.body.position.y += a->base.heightOffset;
    if (NormalizeAngle(32 - state->angle) < 0)
        sprite->angle += state->angularSpeed;
    else
        sprite->angle -= state->angularSpeed;
    angle = NormalizeAngle(sprite->angle);
    Axe_EmitMotionTrail(a);
    if (frame <= 27) {
        if (!Mod(frame, 10))
            state->angularSpeed += 4;
    } else {
        s32 magnitude = angle;
        if (magnitude < 0)
            magnitude = -magnitude;
        if (magnitude <= 95)
            state->angularSpeed = 8;
        else
            state->angularSpeed = 0;
    }
    if (Flags_MaskedWord(0x10, flags)) {
        state->queued = 1;
        RotationPhase_Change(a, SceneActorControl_FinishRotation, 0x33);
        {
            u16 k1 = 0x800;
            state->flags &= ~k1;
        }
        state->trailDuration = 64;
        Axe_SpawnHeadingBurst(a, 14);
    }
}

void SceneActorControl_ResetRotation(struct ChaseActor *actor)
{
    u8 *state = actor->base.context;
    struct RotatingDrawable *drawable = actor->base.drawable;
    s32 angle;
    u8 *timer;
    if (SceneActorControl_TakeFlag(&actor->base.field2B2)) {
        drawable->flags |= 2;
        state[0xEAF] = 30;
    }
    angle = NormalizeAngle(drawable->angle);
    if (angle > 0) {
        s32 step;
        timer = &state[0xEAF];
        step = Div(angle, *timer);
        drawable->angle -= step;
    } else {
        s32 magnitude = angle;
        s32 step;
        if (magnitude < 0)
            magnitude = -magnitude;
        timer = &state[0xEAF];
        step = Div(magnitude, *timer);
        drawable->angle += step;
    }
    --*timer;
    if (state[0xEAF] == 0) {
        Axe_SetAuxiliarySpriteRenderState(actor, state + 0x888, 0x13C);
        actor->base.stepDone = 1;
    }
}

void SceneActorAnimation_SelectProfileEntry(struct RoomActor *, s32);

// A bool8 (u8) result from an inline helper keeps the materialised 0/1 and

// the second test; an int result gets jump-threaded away.

void Axe_OnAnimationReset(struct ChaseActor *actor)
{
    if (SceneActorControl_TakeFlag(&actor->base.field2B2))
        SceneActorAnimation_SelectProfileEntry(actor, 1);
}

// A bool8 (u8) result from an inline helper keeps the materialised 0/1 and

// the second test; an int result gets jump-threaded away.

void Axe_BeginProfileEntry(struct ChaseActor *actor)
{
    if (SceneActorControl_TakeFlag(&actor->base.field2B2)) {
        actor->base.field2C2 = 1;
        SceneActorAnimation_SelectProfileEntry(actor, 0);
    }
}

// confidence=low: turns one octant toward the requested direction on animation ticks.

void Axe_GetDirectionalFrame(u16 x, s32 *a, s32 *b);

static inline void Select(struct RoomActor *actor, u32 selection, u32 variant, u32 flags)
{
    u32 force;
    u8 *restart;
    if (flags & 0x10) {
        force = 0x10;
        flags &= ~0x10;
    } else
        force = 0;
    if (!actor->drawableKind) {
        struct SceneActorAnimationSequenceDrawable *drawable;
        actor->animationIndex = selection;
        drawable = actor->drawable;
        Anim_SetSequence(&drawable->sequence, (void *)drawable->resource, (u16)selection,
                         (u8)variant, (u8)flags);
        restart = &actor->restart;
    } else {
        struct SceneInputWidget *tile =
            &((struct SceneActorAnimationTileDrawable *)actor->drawable)->tile;
        u8 *savedSelection;
        selection += variant;
        savedSelection = &actor->animationIndex;
        restart = &actor->restart;
        if (!force && *savedSelection == selection && !*restart) {
            if (flags == 1 && (tile->flags & 4))
                return;
            if (flags == 0 && !(tile->flags & 4))
                return;
        }
        *savedSelection = selection;
        {
            u32 mode;
            if (flags & 4)
                mode = 6;
            else
                mode = 4;
            Anim_SetKeyframes(&((struct SceneActorAnimationTileDrawable *)actor->drawable)->tile,
                              actor->drawable, (u16)selection, 2, (u16)mode);
            if (flags & 1)
                tile->flags |= 4;
            else
                tile->flags &= ~4;
        }
    }
    *restart = 0;
}

void Anim_SetDurationScale(void *, u32);

void Anim_SetFrameDurationScale(void *, u32);

// confidence=low: turns one octant toward the requested direction on animation ticks.

void Axe_UpdateTurnAnimation(struct RoomActor *actor, u32 events)
{
    u8 *direction = (u8 *)&actor->body;
    struct DisplaySequenceState *sequence;
    if (SceneActorControl_TakeFlag(&actor->field2B2)) {
        s32 current = ((actor->body.direction + 0x70) >> 5) & 7;
        s32 target = ((actor->nextDirection + 0x70) >> 5) & 7;
        if (current == target) {
            actor->body.direction = actor->nextDirection;
            SceneActorChase_SetControl(actor, actor->onStanding, 0);
            actor->stepDone = 1;
            return;
        }
        target -= 8;
        {
            s32 k1 = 4;
            if (((u8)((current - target) & 7)) <= k1) {
                u32 *p = &actor->behaviorFlags;
                u32 mask = ~0x10000;
                *p &= mask;
            } else {
                u32 mask = 0x10000;
                actor->behaviorFlags |= mask;
            }
        }
    }
    {
        struct ChaseActor *state = (void *)actor;
        sequence = &((struct SceneActorAnimationSequenceDrawable *)actor->drawable)->sequence;
        {
            u16 *scale;
            if (SceneActorChase_QueryMotionFlags(state, 0xA02F)) {
                state->base.field2C2 = 64;
                scale = &state->base.field2C2;
            } else {
                u16 *p = &state->base.field2C2;
                u16 value = *p;
                scale = p;
                if (!value)
                    *scale = 64;
            }
            if (!actor->drawableKind)
                Anim_SetDurationScale(
                    &((struct SceneActorAnimationSequenceDrawable *)actor->drawable)->sequence,
                    *scale);
            else
                Anim_SetFrameDurationScale(
                    &((struct SceneActorAnimationTileDrawable *)actor->drawable)->tile, *scale);
        }
    }
    if (events & 4) {
        struct SceneActorProfileEntry *entry =
            (void *)SceneActorProfiles_GetEntryAddress(actor->profileId, actor->profileMode, 1);
        u32 variant;
        u32 flags;
        u32 selection;
        SceneActorChase_SetControl(actor, actor->onEntry, 2);
        if (Flags_MaskedWord(0x10000, &actor->behaviorFlags))
            direction[9] += 32;
        else
            direction[9] -= 32;
        Axe_GetDirectionalFrame(direction[9], &variant, &flags);
        sequence->frameCount = 0;
        selection = entry->value;
        Select(actor, selection, variant, flags);
    }
}

static inline u16 Flag(struct RoomActor *actor, u16 mask)
{
    u16 *p = &actor->reactionFlags;
    return *p = mask | *p;
}

// confidence=low: selects the axe actor's waiting animation and marks the wait complete after 60 ticks.

void Axe_UpdateWaitingAnimation(struct RoomActor *actor, s32 frame)
{
    struct SceneActorProfileEntry *entry =
        (void *)SceneActorProfiles_GetEntryAddress(actor->profileId, actor->profileMode, 11);
    u32 variant, flags, selection;
    if (actor->field2B2)
        actor->field2B2 = 0;
    Axe_GetDirectionalFrame(actor->body.direction, &variant, &flags);
    selection = entry->value;
    Select(actor, selection, variant, flags);
    if (frame <= 29)
        Flag(actor, 32);
    if (frame > 59)
        actor->stepDone = 1;
}

// confidence=low: converges saved motion, then enables the axe recovery behavior.

void Axe_ClearForcedMovement(struct ChaseActor *s);

void Axe_ResetMovementFlags(struct ChaseActor *actor);

static inline s32 Clamp(s32 v)
{
    if (v > 200)
        return 200;
    if (v < -200)
        return -200;
    return v;
}

static inline u8 AtRest(s32 x, s32 z)
{
    if (!x && !z)
        return 1;
    return 0;
}

static inline void SetFlags(struct ChaseActor *a, u32 m)
{
    u32 v = m;
    v |= a->base.motionFlags;
    a->base.motionFlags = v;
}

static inline void RestoreHistory(struct ChaseActor *a, u16 v)
{
    a->group.common.gate.counter = v;
}

void Axe_ConvergeRecoveryMotion(struct ChaseActor *actor)
{
    struct Axe *context = actor->base.context;
    s32 x, z;
    s16 *priorX, *priorZ;
    if (SceneActorControl_TakeFlag(&actor->base.field2B2)) {
        SetFlags(actor, 0x20);
        SceneActorAnimation_SelectProfileEntry((void *)actor, 3);
        Axe_ClearForcedMovement(actor);
        Axe_ResetMovementFlags(actor);
    }
    {
        s32 current = actor->base.reactionStart.x;
        s16 *field = &actor->base.reactionEnd.x;
        s32 difference = current - *field;
        priorX = field;
        x = SceneActorDisplacement_Quarter(difference);
    }
    {
        s32 current = actor->base.reactionStart.z;
        s16 *field = &actor->base.reactionEnd.z;
        s32 difference = current - *field;
        priorZ = field;
        z = SceneActorDisplacement_Quarter(difference);
    }
    x = Clamp(x);
    z = Clamp(z);
    actor->base.body.motion.x += x;
    actor->base.body.motion.z += z;
    *priorX += x;
    *priorZ += z;
    if (AtRest(x, z)) {
        actor->base.stepDone = 1;
        {
            struct SignedFlag {
                s32 bits : 16;
            };
            struct SignedFlag *f = (void *)&actor->base.motionFlags;
            s32 m = ~0x20;
            m &= (u16)f->bits;
            f->bits = m;
        }
        {
            u32 *f = &actor->base.behaviorFlags;
            s32 m = ~0x80;
            *f &= m;
        }
        SceneActorChase_SetStatusFlags(actor, 0x100000);
        Axe_ResetMovementFlags(actor);
        WordFlags_Set(0x10, &context->flagsE4C);
        RestoreHistory(actor, actor->group.common.gate.duration);
        SceneActorTerrain_EnterState(actor, 0, 2, 9);
    }
}

void ActorShadow_Disable(void *);

void Axe_UpdateAttackColliderPositions(struct ChaseActor *actor);

void Axe_ReleaseAuxiliarySprite(struct ChaseActor *actor);

static inline u8 HasAuxiliary(const struct Axe *state)
{
    return state->auxiliarySprite.unknown4[0] != 0;
}

// confidence=low: spins the auxiliary axe after damage and releases it after 30 frames.

void Axe_UpdateDamageSpin(struct ChaseActor *actor, s32 frame)
{
    struct SceneActorRegistryControlGroup *group = &actor->group.common;
    struct Axe *context = actor->base.context;
    if (SceneActorControl_TakeFlag(&actor->base.field2B2)) {
        struct Axe *state = actor->base.context;
        struct SceneActorRegistryControl *control = &group->control;
        u32 *field = &actor->base.behaviorFlags;
        u32 mask = ~0x141C0;
        *field &= mask;
        control->counter = 0;
        control->field14 = 0;
        control->timer = 0;
        SceneActorAnimation_SelectProfileEntry((void *)actor, 3);
        Sound_PlaySong(318);
        actor->base.colliderA.responseValue = 0;
        ActorStatus_ClearDrawableFlags(actor, 1);
        SceneActorTerrain_EnterState(actor, 3, 0, 0);
        actor->base.attachmentTimer = 0;
        if (HasAuxiliary(state)) {
            u32 *flags = &state->auxiliarySprite.flags;
            *flags |= 2;
            state->flagsCAB = 0;
            state->trailHeading = 0;
            state->trailPosition = state->auxiliarySprite.position;
            {
                u32 angle = state->trailStartAngle;
                state->trailAngle = angle;
            }
            state->trailSpinRate = 4;
            ActorShadow_Disable(state->auxiliaryShadow);
            *flags = 1;
        }
        state->damageSpinStarted = 1;
        state->reactionHoldTimer = 0;
    }
    if (context->reactionHoldTimer) {
        --context->reactionHoldTimer;
        HalfwordFlags_Set(0x20, &actor->base.reactionFlags);
    }
    if (context->auxiliarySprite.unknown4[0]) {
        u32 previous = context->trailAngle;
        s32 next = previous - context->trailSpinRate;
        context->trailAngle = next;
        if ((s32)previous < next)
            Sound_PlaySong(361);
        if ((frame & 1) && context->trailSpinRate <= 19)
            context->trailSpinRate++;
        ActorPhase_EmitTrailSprite(actor, &context->trailPosition, context->trailAngle);
        Axe_UpdateTrailColliders(actor);
        Axe_UpdateAttackColliderPositions(actor);
        if (frame > 30)
            Axe_ReleaseAuxiliarySprite(actor);
    }
}

void Axe_ReleaseAndIdle(struct ChaseActor *actor)
{
    Axe_ReleaseAuxiliarySprite(actor);
    Chase_SetCtl(actor, SceneActorControl_OnIdle, 0x10);
    Chase_SetBehavior(actor, 4);
}

void *Bestiary_GetEntryForActor(u32, u32);

void Bestiary_UnlockEntry(void *);

void SceneActorBehavior_CallTerrainScript(struct RoomActor *);

void SceneActorPlacement_PlayCategoryEventSound(struct ChaseActor *);

u8 Sprite_BlinkUntilExpired(void *, s32, s32, s32);

void CpuSet(const void *, void *, u32);

void SparkParticle_Spawn(const void *, s32, const void *, s32, s32, s32, s32, s32);

static inline s32 Scale_UpdateDefeat(s32 x)
{
    s32 result;
    if (x >= 0)
        result = x >> 12;
    else
        result = -(-x >> 12);
    return result;
}

void Axe_UpdateDefeat(struct ChaseActor *actor, s32 frame)
{
    struct {
        u32 zero;
        s32 values[8];
    } buf;
    struct ContactPackedVector range;
    struct ContactPosition offset;
    struct ContactPosition position;
    struct ScriptUtilityArguments arguments;
    struct Axe *context = actor->base.context;
    u8 *drawable = actor->base.drawable;
    u16 script;
    if (SceneActorControl_TakeFlag(&actor->base.field2B2)) {
        SceneActorAnimation_SelectProfileEntry(actor, 10);
        SceneActorPlacement_PlayCategoryEventSound((void *)actor);
        {
            struct ContactCollider *c = &actor->base.colliderB;
            c->flags |= 4;
        }
        {
            struct ContactCollider *c = &actor->base.colliderA;
            c->flags |= 4;
        }
        {
            u32 mask = 0x800;
            actor->base.placementFlags = mask | actor->base.placementFlags;
        }
        context->sparkStep = 0;
        Axe_ReleaseAuxiliarySprite(actor);
    }
    if (Mod(frame, 2) == 0) {
        s32 x, z, x2;
        range.x = 60;
        range.y = 60;
        range.z = 40;
        position = actor->base.body.position;
        MonsterInit_ClearPosition(&buf.zero, &offset);
        switch (context->sparkStep) {
            case 0:
                offset.x = -120;
                offset.z = -120;
                context->sparkStep++;
                Sound_PlaySong(0x16E);
                break;
            case 1:
                offset.x = -75;
                offset.z = -75;
                context->sparkStep++;
                break;
            case 2:
                offset.x = -30;
                offset.z = -30;
                context->sparkStep++;
                break;
            case 3:
                offset.x = 15;
                offset.z = 15;
                context->sparkStep++;
                break;
            case 4:
                offset.x = 60;
                offset.z = 60;
                context->sparkStep = 0;
                break;
        }
        x = Scale_UpdateDefeat(offset.x * gUnk_086149C4[(drawable[6] + 64) & 255]);
        z = Scale_UpdateDefeat(offset.z * gUnk_086149C4[drawable[6]]);
        position.x = x + z + position.x;
        x2 = Scale_UpdateDefeat(-(offset.x * gUnk_086149C4[drawable[6]]));
        z = Scale_UpdateDefeat(offset.z * gUnk_086149C4[(drawable[6] + 64) & 255]);
        position.z = x2 + z + position.z;
        SparkParticle_Spawn(&position, 2, &range, 0, 0, 22, 2, 1);
    }
    if (!Sprite_BlinkUntilExpired(actor->base.drawable, frame, 10, 30))
        return;
    actor->base.stepDone = 1;
    SceneActorBehavior_CallTerrainScript((void *)actor);
    script = (u16)actor->base.script1F8;
    if (script) {
        buf.values[0] = actor->base.body.position.x;
        buf.values[1] = actor->base.body.position.y;
        buf.values[2] = actor->base.body.position.z;
        buf.values[3] = actor->base.parameters1FC[0];
        buf.values[4] = actor->base.parameters1FC[1];
        buf.values[5] = actor->base.parameters1FC[2];
        buf.values[6] = actor->base.hitOwners;
        buf.values[7] = actor->base.body.id;
        arguments.count = 8;
        arguments.values = buf.values;
        Script_Call(script, &arguments);
    }
    if (!SceneActorChase_QueryMotionFlags(actor, 0x4000) && (u32)(actor->base.category - 11) > 2)
        Bestiary_UnlockEntry(
            Bestiary_GetEntryForActor(actor->base.category, actor->base.paletteOffset));
    Bestiary_UnlockEntry(Bestiary_GetEntryForActor(actor->base.category, context->variant));
    actor->base.attachmentTimer = 0;
    SceneActorChase_SetTypedControl(actor, SceneActorControl_OnIdle, 16);
    SceneActorTerrain_EnterState(actor, 1, 0, 2);
}

static inline s32 Normalize(s32 a)
{
    s32 r = a;
    if (a > 128)
        r -= 256;
    else if (a < -128)
        r += 256;
    return r;
}

static inline void Polar(struct ContactPosition *v, s32 angle, s32 radius)
{
    v->x = Scale(gUnk_086149C4[(angle + 64) & 255] * radius);
    v->y = 0;
    v->z = Scale(gUnk_086149C4[angle & 255] * radius);
}

static inline s32 Abs(s32 x)
{
    s32 result = x;
    if (result < 0)
        result = -result;
    return result;
}

static inline s32 Eighth(s32 x)
{
    s32 result;
    if (x >= 0)
        result = x >> 3;
    else
        result = -(-x >> 3);
    return result;
}

static inline void FromPlayer(u8 **player, struct ContactPosition *out, struct ChaseActor *owner)
{
    const struct ContactPosition *target = (void *)(*player + 0x30);
    const struct ContactPosition *actor = &owner->base.body.position;
    out->x = actor->x - target->x;
    out->y = actor->y - target->y;
    out->z = actor->z - target->z;
}

static inline void ToPlayer(u8 **player, struct ContactPosition *out,
                            const struct ContactPosition *actor)
{
    const struct ContactPosition *target = (void *)(*player + 0x30);
    out->x = target->x - actor->x;
    out->y = target->y - actor->y;
    out->z = target->z - actor->z;
}

void Axe_UpdatePursuit(struct ChaseActor *actor)
{
    struct ContactPosition delta;
    struct ContactPosition velocity;
    struct Axe *context = actor->base.context;
    s32 angle, step;
    if (SceneActorControl_TakeFlag(&actor->base.field2B1)) {
        SceneActorChase_SetControl(actor, actor->base.onAlternateEntry, 1);
        *(u32 *)actor->base.drawable |= 2;
        context->distance = 512;
    }
    Axe_EmitMotionTrail(actor);
    if (context->distance > actor->base.tracking.distance) {
        s32 difference;
        FromPlayer(&gPlayers, &delta, actor);
        angle = Math_Atan2(delta.x, delta.z);
        difference = Normalize(((angle + 64) & 255) - actor->base.body.direction);
        if (difference < 0)
            difference = -difference;
        if (difference <= 63)
            angle += 64;
        else
            angle += 192;
        angle &= 255;
        step = Div(32, 8);
        if (step <= 0)
            step = 2;
    } else {
        s32 difference, magnitude;
        ToPlayer(&gPlayers, &delta, &actor->base.body.position);
        angle = Math_Atan2(delta.x, delta.z);
        difference = Normalize(angle - actor->base.body.direction);
        magnitude = Abs(difference);
        {
            s32 result = Eighth(magnitude);
            step = result;
            if (result <= 0)
                result = 2;
            step = result;
        }
    }
    if (Normalize(angle - actor->base.body.direction) > 0)
        actor->base.body.direction += step;
    else
        actor->base.body.direction -= step;
    Polar(&velocity, actor->base.body.direction, 32);
    *((u8 *)actor->base.drawable + 6) = 159 - actor->base.body.direction;
    actor->base.body.motion = velocity;
    actor->base.counter2AC++;
}

// Remaining: counter offset is kept in sb across Mod rather than rematerialized at the final store.

struct DampedMotionState {
    u8 unknown0[0xF14];
    s16 x;
    u8 unknownF16[2];
    s16 z;
};

void SceneActorControl_ApplyDampedMotion(struct ChaseActor *actor)
{
    struct DampedMotionState *state = actor->base.context;
    if (SceneActorControl_TakeFlag(&actor->base.field2B1))
        RotationPhase_Change(actor, actor->base.onEntry, 2);
    actor->base.body.motion.x += state->x;
    actor->base.body.motion.z += state->z;
    if (Mod(actor->base.counter2AC, 4) == 0) {
        state->x >>= 1;
        state->z >>= 1;
    }
    if (actor->base.stepDone) {
        Chase_SetBehavior(actor, 0);
    } else {
        ++actor->base.counter2AC;
    }
}

u8 Axe_PickAttack(void *);

void ActorPhase_UpdateSpriteCollapse(void);

// confidence=low: speed and phase duration depend on remaining health.

void Axe_UpdateHealthPacedMotion(struct ChaseActor *actor)
{
    struct Axe *context = actor->base.context;
    s32 speed;
    if (SceneActorControl_TakeFlag(&actor->base.field2B1))
        SceneActorChase_SetControl(actor, actor->base.onStanding, 0);
    if ((u8)Callback_InvokeActor(actor, actor->base.contactQuery.opaque) && Axe_PickAttack(actor))
        return;
    if (actor->base.health <= context->healthThreshold) {
        speed = 8;
        if ((s32)actor->base.counter2AC > 400) {
            SceneActorChase_SetControl(actor, ActorPhase_UpdateSpriteCollapse, 35);
            SceneActorTerrain_EnterState(actor, 0, 0, 4);
            return;
        }
    } else if (actor->base.health <= context->lowHealthThreshold) {
        speed = 6;
        if ((s32)actor->base.counter2AC > 500) {
            SceneActorChase_SetControl(actor, ActorPhase_UpdateSpriteCollapse, 35);
            SceneActorTerrain_EnterState(actor, 0, 0, 4);
            return;
        }
    } else {
        speed = 4;
        if ((s32)actor->base.counter2AC > 600) {
            SceneActorTerrain_EnterState(actor, 0, 0, 4);
            return;
        }
    }
    Polar(&actor->base.body.motion, actor->base.tracking.heading, speed);
    actor->base.counter2AC++;
}

struct AxeFlags16 {
    u16 bits : 16;
};

void Axe_UpdateCollapsePhase(struct ChaseActor *actor)
{
    if (Chase_TakeArrived(actor)) {
        SceneActorChase_SetControl(actor, ActorPhase_UpdateSpriteCollapse, 0x23);
        {
            u32 flag = 0x1800;
            flag |= actor->base.placementFlags;
            actor->base.placementFlags = flag;
        }
    }
    if (actor->base.stepDone) {
        struct AxeFlags16 *flags = (void *)&actor->base.placementFlags;
        u32 mask = ~0x1800;
        flags->bits &= mask;
        SceneActorChase_SetControl(actor, actor->base.onStanding, 0);
        Chase_SetBehavior(actor, 3);
    } else {
        actor->base.counter2AC++;
    }
}

void SceneActorControl_UpdatePausePhase(struct ChaseActor *actor)
{
    struct Axe *control = actor->base.context;
    if (SceneActorControl_TakeFlag(&actor->base.field2B1)) {
        SceneActorChase_SetControl(actor, actor->base.onStanding, 0);
        control->hoverPhase = 0;
    }
    {
        s32 *c = (s32 *)&actor->base.counter2AC;
        if (*c > 60)
            Chase_SetBehavior(actor, 3);
        ++*c;
    }
}

extern void *gUnk_08606444[];

void Axe_InitMotionTable(struct ChaseActor *actor)
{
    actor->base.callbacks284 = gUnk_08606444;
}

#define gHoverSineTable gUnk_086149C4

void SceneActorControl_UpdateHoverHeight(struct ChaseActor *actor)
{
    struct Axe *context = actor->base.context;
    if (!(SceneActorChase_QueryMotionFlags(actor, 0x8000))) {
        s32 height = Scale(gHoverSineTable[context->hoverPhase] << 5);
        struct Player *player = gPlayers[0];
        u32 y = height + 220;
        y += (u16)player->position.y;
        actor->base.body.position.y = y;
        context->auxiliarySprite.position.y = y;
        context->hoverPhase += 2;
    }
}

u8 *Path_GetPointData(u8 *);

struct E {
    u8 pad0[2];
    u8 a;
    u8 b;
    u8 *p;
    u8 *q;
};

struct S080FA7D0 {
    u8 filler[0x448];
    struct E e;
};

void Axe_SelectRandomRoutePoint(struct S080FA7D0 *s)
{
    struct E *e = &s->e;
    u8 v;

    gRandomIndex = (gRandomIndex + 1) & 0x3FF;
    v = Mod(*(u16 *)(((u8 *)gRandomTable) + (gRandomIndex << 1)), *e->p);
    if (v == e->a)
        v = Mod(v + 1, *e->p);
    e->a = v;
    e->b = 0;
    e->q = Path_GetPointData(e->p) + (e->a << 3);
}

void SceneActorCollision_InitBody(struct ChaseActor *, void *, void *, void *, void *);

void Axe_OnBodyContact(void);

void Axe_OnAttackColliderContact(void);

void Axe_InitColliders(struct ChaseActor *actor)
{
    struct Axe *blk = actor->base.context;
    struct ContactPackedVector bodySize, bodyOff, atkSize, atkOff;
    s32 i;
    u16 f = 0x2401;
    u32 k;
    bodySize.x = 0x18;
    bodySize.y = 0x46;
    bodySize.z = 0x18;
    bodyOff.x = 0;
    bodyOff.y = 0;
    bodyOff.z = 0;
    SceneActorCollision_InitBody(actor, &bodySize, &bodyOff, Axe_OnBodyContact, actor);
    for (i = 0; i < 2; i++) {
        struct ContactCollider *c = &blk->extraBody[i];
        Collider_Init(c, actor->base.body.id, 0x4001, 0, 16, &bodySize, &bodyOff);
        Collider_SetBodyProperties(c, SceneActorCollision_ReadBodyField3C(actor),
                                   SceneActorCollision_ReadBodyReactionFlags(actor),
                                   SceneActorCollision_ReadBodyField34(actor));
        Collider_SetPosition(c, &actor->base.body.position, 0);
        Collider_SetCallback(c, Axe_OnBodyContact, actor);
        Collider_Refresh(c);
    }
    atkSize.x = 0x18;
    atkSize.y = 0x40;
    atkSize.z = 0x18;
    atkOff.x = 0;
    atkOff.y = 0;
    atkOff.z = 0;
    {
        struct ContactCollider *c = &actor->base.colliderB;
        Collider_Init(c, actor->base.body.id, f, 0, 16, &atkSize, &atkOff);
        Collider_SetAttackProperties(c, SceneActorCollision_ReadAttackField3C(actor, 0),
                                     SceneActorCollision_ReadAttackField40(actor, 0), 0,
                                     SceneActorCollision_ReadAttackReactionFlags(actor, 0),
                                     SceneActorCollision_ReadAttackField44(actor, 0));
        Collider_SetPosition(c, &actor->base.body.position, 0);
        Collider_SetCallback(c, Axe_OnAttackColliderContact, actor);
    }
    k = 0x24010000;
    for (i = 0; i < 2; i++) {
        struct ContactCollider *c = &blk->extraAttack[i];
        Collider_Init(c, actor->base.body.id, k >> 16, 0, 16, &atkSize, &atkOff);
        Collider_SetAttackProperties(c, SceneActorCollision_ReadAttackField3C(actor, 0),
                                     SceneActorCollision_ReadAttackField40(actor, 0), 0,
                                     SceneActorCollision_ReadAttackReactionFlags(actor, 0),
                                     SceneActorCollision_ReadAttackField44(actor, 0));
        Collider_SetPosition(c, &actor->base.body.position, 0);
        Collider_SetCallback(c, Axe_OnAttackColliderContact, actor);
    }
}

s32 Axe_GetActorCategory(void)
{
    return 4;
}

void Axe_SetMovementEndpoints(struct ChaseActor *actor, struct ContactPosition *end)
{
    struct Axe *context = actor->base.context;
    context->curveStart = actor->base.body.position;
    context->curveEnd = *end;
    context->curveTime = 0;
}

struct AxeMoveNode {
    u8 unknown0[12];
    struct ContactPosition position;
};

u8 Axe_StepCurveMotion(struct ChaseActor *actor)
{
    struct Axe *ctx = actor->base.context;
    struct ContactPosition start, end;
    start = ctx->curveStart;
    start.y = 0;
    end = ctx->curveEnd;
    end.y = 0;
    MovementCurve_InterpolatePosition(&((struct AxeMoveNode *)actor->group.route)->position, &start,
                                      &end, 1, ctx->curveTime, 0, ctx->curve);
    if (ctx->curveTime > 1023)
        return 1;
    ctx->curveTime += 128;
    return 0;
}

s32 SceneActorProfiles_FindOrAdd(u32, u32);

void Axe_InitProfile(struct ChaseActor *actor)
{
    actor->base.profileId = SceneActorProfiles_FindOrAdd(actor->base.category, 0);
}

#define gHoverAnimationData gUnk_0824E318

extern const u8 gUnk_0824E318[];

void SceneActorReaction_ResetResponseState(void *, const void *, u32);

void SceneActorControl_InitializeHoverState(struct ChaseActor *actor)
{
    struct Axe *context = actor->base.context;
    // concession-ok(const-local): The CpuSet position clears use the same fixed source word.
    u32 zero;
    context->curveTime = 0;
    context->hoverPhase = 0;
    context->hoverEnabled = 1;
    context->curve = 0;
    context->flagsE4C = actor->base.saved.privateFlags;
    context->target = *(struct ContactPosition *)&gPlayers[0]->position;
    MonsterInit_ClearPosition(&zero, &context->curveStart);
    MonsterInit_ClearPosition(&zero, &context->curveEnd);
    MonsterInit_ClearPosition(&zero, (struct ContactPosition *)context->clearF14);
    Memory_Clear(actor->group.destination, 8);
    SceneActorReaction_ResetResponseState(&actor->group.saved.control, gHoverAnimationData, 0xD4);
    SceneActorAnimation_SelectDirectionalEntry((void *)actor, 0);
}

void Axe_ResetMovementFlags(struct ChaseActor *actor)
{
    struct Axe *context = actor->base.context;
    u32 m = 0x100000;
    u32 *p = &actor->base.statusFlags;
    u32 *p2;
    u32 m2;
    *p = *p | m;
    p2 = &context->flagsE4C;
    m2 = ~0x11;
    *p2 = *p2 & m2;
    actor->base.colliderA.flags &= ~4;
}

void Axe_GetDirectionalFrame(u16 x, s32 *a, s32 *b)
{
    s32 v = ((x + 0x70) >> 5) & 7;
    s32 r;
    if (v > 4) {
        *a = 8 - v;
        r = 1;
    } else {
        *a = v;
        r = 0;
    }
    *b = r;
}

void Axe_SetMovementEndpoints(struct ChaseActor *actor, struct ContactPosition *end);

u8 Axe_StepCurveMotion(struct ChaseActor *actor);

static inline void Fill_BeginMoveToPosition(void *dst, u32 *tmp, u32 v)
{
    *tmp = v;
    CpuSet(tmp, dst, 0x05000002);
}

void Axe_BeginMoveToPosition(struct ChaseActor *actor, struct ContactPosition *target)
{
    struct Axe *context = actor->base.context;
    u32 stack[3];
    struct ContactPosition *delta;
    Axe_SetMovementEndpoints(actor, target);
    Axe_StepCurveMotion(actor);
    Chase_SetBehavior(actor, 1);
    delta = (struct ContactPosition *)(stack + 1);
    Chase_Delta(delta, &actor->base.body.position, target);
    actor->base.nextDirection = Math_Atan2(delta->x, delta->z);
    Fill_BeginMoveToPosition(context->clearF14, stack, 0);
}

void Axe_ClearForcedMovement(struct ChaseActor *s)
{
    u16 *hp = &s->base.motionFlags;
    u32 hm = 0xFFFFFEFF;
    u32 *p;
    u32 m;
    struct SceneActorChaseTracking *t;
    u32 bm;
    u16 zero;
    u16 hv;
    hv = *hp & hm;
    zero = 0;
    *hp = hv;
    t = &s->base.tracking;
    bm = ~8;
    t->flags = t->flags & bm;
    p = &((struct Axe *)s->base.context)->flagsE4C;
    m = ~0x10;
    *p = *p & m;
    s->group.common.gate.counter = zero;
}

void Axe_SelectRandomRoutePoint(struct S080FA7D0 *s);

void Axe_BeginMoveToPosition(struct ChaseActor *actor, struct ContactPosition *target);

struct AxeVec {
    s16 x, y, z;
};

static inline u32 CellIndex(struct AxeVec *p)
{
    s32 x = p->x >> 8, z = p->z >> 8;
    if (x < 0 || z < 0 || (u32)x >= gTerrainWidth || (u32)z >= gTerrainHeight)
        return 0;
    return gTerrainMap->rows[z] + x;
}

static inline u32 CellHeight(struct AxeVec *position, u32 cell)
{
    u32 shape, height;
    u8 *data = TerrainModifiers_FindCellOverride(cell, 1);
    if (data)
        data += 4;
    else {
        struct EngineMap *map = gTerrainMap;
        u32 offset = cell * 4 + 12;
        data = map->cells + offset;
    }
    shape = *data >> 4;
    height = (*data & 15) << 8;
    if (shape == 1)
        goto slopeZ;
    if (shape == 2)
        goto slopeX;
    goto done;
slopeZ:
    height -= *((u8 *)position + 4);
    goto done;
slopeX:
    height -= *(u8 *)position;
done:
    return height;
}

static inline u32 Flag_UpdateRouteWander(void *base, u32 maskInput, u32 off)
{
    u32 mask = maskInput;
    return off == 0x2D4 ? *(u16 *)((u8 *)base + off) & mask : *(u32 *)((u8 *)base + off) & mask;
}

static inline u8 IsDown(struct ChaseActor *actor)
{
    if (actor && (Flag_UpdateRouteWander(actor, 0x40, 0x2D4) ||
                  Flag_UpdateRouteWander(actor, 0x20000, 0x2D0) || actor->base.health <= 0))
        return 1;
    return 0;
}

static inline u8 HasMark(struct Axe *context)
{
    if (Flag_UpdateRouteWander(context, 0x10, 0xE4C))
        return 1;
    return 0;
}

void Axe_UpdateRouteWander(struct ChaseActor *actor)
{
    struct Axe *context = actor->base.context;
    struct SceneActorChaseGroup *group = &actor->group;
    struct AxeVec targetStorage;
    struct AxeVec *target;
    struct SceneActorChaseRoutePoint *point;
    if (IsDown(actor))
        return;
    if (Flag_UpdateRouteWander(actor, 0x100, 0x2D4))
        return;
    if (!HasMark(context)) {
        group->common.gate.counter = 0;
        return;
    }
    if (group->common.gate.counter != 0) {
        group->common.gate.counter--;
        return;
    }
    Axe_SelectRandomRoutePoint(actor);
    target = &targetStorage;
    point = group->homeRoute.point;
    targetStorage.x = point->x;
    target->z = point->z;
    target->y = CellHeight(target, CellIndex(&targetStorage));
    Axe_BeginMoveToPosition(actor, &targetStorage);
    Axe_ClearForcedMovement(actor);
    Axe_ResetMovementFlags(actor);
    SceneActorTerrain_EnterState(actor, 0, 0, 2);
}

// confidence=low: initializes scripted cooldown lengths and the health threshold.

void Axe_InitAttackCooldowns(struct ChaseActor *actor)
{
    struct Axe *state = actor->base.context;
    s32 i;
    s32 health, quarter;
    s16 value;
    s16 *healthField;
    s32 zero;
    if (Script_SeekToKeyword(0x50) != 0) {
        for (i = 0; i <= 3; i++) {
            state->attackDelays[i] = Script_GetPc() ? Script_GetValue() : 0;
        }
    }
    healthField = &actor->base.health;
    zero = 0;
    for (i = 3;; --i) {
        if (i < 0) {
            quarter = *healthField;
            health = quarter;
            break;
        }
        state->attackCooldowns[i] = zero;
    }
    value = quarter;
    if (value >= 0)
        quarter = value >> 2;
    else
        quarter = -(-value >> 2);
    state->lowHealthThreshold = health - quarter;
}

void Axe_TickCooldowns(struct ChaseActor *actor)
{
    u16 *p = ((struct Axe *)actor->base.context)->attackCooldowns;
    s32 i = 3;
    do {
        if (*p)
            (*p)--;
        p++;
    } while (--i >= 0);
}

u8 Axe_PickAttack(void *actorIn)
{
    struct ChaseActor *actor = (struct ChaseActor *)actorIn;
    struct Axe *q = actor->base.context;
    if (actor->base.health <= q->healthThreshold && q->attackCooldowns[3] == 0) {
        SceneActorTerrain_EnterState(actor, 0, 2, 8);
        return 1;
    }
    if (q->attackCooldowns[0] == 0 && actor->base.tracking.distance <= 0xFFFE) {
        s32 a = actor->base.tracking.heading;
        s32 d = a - 0xa0;
        s32 e;
        if (d > 0x80)
            d = a - 0x1a0;
        else if (d < -0x80)
            d = a + 0x60;
        e = d;
        if (e < 0)
            e = -e;
        if (e <= 0x29)
            SceneActorTerrain_EnterState(actor, 0, 2, 0);
        else
            SceneActorTerrain_EnterState(actor, 0, 2, 2);
        return 1;
    }
    if (q->lowHealthThreshold >= actor->base.health) {
        if (q->attackCooldowns[1] == 0) {
            SceneActorTerrain_EnterState(actor, 0, 2, 4);
            return 1;
        }
        if (q->attackCooldowns[2] == 0) {
            SceneActorTerrain_EnterState(actor, 0, 2, 6);
            return 1;
        }
    }
    return 0;
}

void Axe_InitRoutePointHistory(struct ChaseActor *actor)
{
    struct Axe *q = actor->base.context;
    s32 i;
    for (i = 0; i < 3; i++)
        q->recentRoutePoints[i] |= 0xff;
    q->recentRoutePointIndex = 0;
}

void Axe_RecordRoutePoint(struct ChaseActor *actor, u32 x)
{
    struct Axe *q = actor->base.context;
    q->recentRoutePoints[q->recentRoutePointIndex] = x;
    q->recentRoutePointIndex++;
    if (q->recentRoutePointIndex > 2)
        q->recentRoutePointIndex = 0;
}

u8 Axe_IsRoutePointRecent(struct ChaseActor *actor, s32 x)
{
    struct Axe *q = actor->base.context;
    s32 i;
    for (i = 0; i < 3; i++) {
        if ((s8)q->recentRoutePoints[i] == x)
            return 1;
    }
    return 0;
}

inline static u8 Actor_IsInactive(u8 *c)
{
    u32 mask;
    u32 *p;
    u32 off;
    if (!c)
        goto zero;
    mask = 0x40;
    off = 0xB5;
    off <<= 2;
    p = (u32 *)(c + off);
    if ((*((u16 *)p)) & mask)
        goto one;
    mask = 0x80;
    mask <<= 10;
    off -= 4;
    p = (u32 *)(c + off);
    if ((*p) & mask)
        goto one;
    if ((*((s16 *)(c + 0x9C))) > 0)
        goto zero;
one:
    return 1;
zero:
    return 0;
}

s32 SceneActorControl_CanAdvanceHover(struct ChaseActor *actor)
{
    if (!Actor_IsInactive((void *)actor)) {
        struct Axe *context = actor->base.context;
        context->target = *(struct ContactPosition *)&gPlayers[0]->position;
        {
            if (!(SceneActorChase_QueryMotionFlags(actor, 0x100)) &&
                actor->base.tracking.distance < actor->base.tracking.limit)
                goto one;
        }
    }
    return 0;
one:
    return 1;
}

void Axe_OnReaction(void)
{
}

struct AxeFlashState {
    u8 unk_00[0x908];
    u16 palette[16];
    u8 unk_928[0xE4C - 0x928];
    u32 flags;
    u8 unk_E50[0xECE - 0xE50];
    u8 timer;
    s8 level;
    u8 start;
    u8 stop;
    u8 unk_ED2[4];
    u8 reset;
};

struct AxeFlashDrawable {
    struct ContactSwarmSprite sprite;
    struct DisplayTrackGraphics graphics;
};

/* Fade the attack palette, then restore the actor palette when the level reaches zero. */

void Axe_UpdateAttackFlashFade(struct ChaseActor *actor)
{
    struct AxeFlashState *state = actor->base.context;
    u32 *flags;
    if (state->start) {
        state->level = 0;
        state->timer = 0;
        {
            u32 bit = 0x80;
            state->flags |= bit;
        }
        state->start = 0;
    }
    {
        u8 stop = state->stop;
        flags = &state->flags;
        if (stop) {
            {
                u32 bit = 0x100;
                *flags |= bit;
            }
            *flags &= ~0x80;
            state->level = 3;
            state->timer = 0;
            state->stop = 0;
        }
    }
    if (state->reset) {
        {
            u32 bit = 0x100;
            *flags |= bit;
        }
        *flags &= ~0x80;
        {
            s32 lv = state->level;
            if (lv > 3)
                lv = 3;
            else if (lv < 0)
                lv = 0;
            state->level = lv;
        }
        state->timer = 0;
        state->reset = 0;
    }
    {
        if (WordFlags_Has(0x180, flags)) {
            ((struct AxeFlashDrawable *)actor->base.drawable)->graphics.defaultPalette = 0x13F;
            ((struct AxeFlashDrawable *)actor->base.drawable)->graphics.palette = state->palette;
            switch (state->level) {
                case 0:
                    state->palette[1] = 0;
                    break;
                case 1:
                    state->palette[1] = 16;
                    break;
                case 2:
                    state->palette[1] = 24;
                    break;
                case 3:
                    state->palette[1] = 31;
                    break;
            }
            if (++state->timer > 9) {
                if (!WordFlags_Has(0x80, flags)) {
                    if (--state->level <= 0) {
                        void *drawable =
                            &((struct AxeFlashDrawable *)actor->base.drawable)->graphics;
                        u16 *palette = &actor->base.basePaletteBank;
                        u16 *offset = &actor->base.paletteOffset;
                        AnimData_SelectPaletteBank(drawable, *palette + *offset);
                        {
                            u32 clr = ~0x100;
                            *flags &= clr;
                        }
                        return;
                    }
                } else {
                    if (state->level < 3)
                        state->level++;
                }
                state->timer = 0;
            }
        }
    }
}

struct S080FB4EC_Ctx {
    u8 unk0[0x90A];
    u16 unk90A;
    u8 unk90C[0xECE - 0x90C];
    u8 unkECE;
    u8 unkECF;
    u8 unkED0;
    u8 unkED1;
};

struct S080FB4EC_Drawable {
    u8 unk0[0x32];
    u16 unk32;
    u8 unk34[0x38 - 0x34];
    void *unk38;
};

struct S080FB4EC_Actor {
    void *object;
    struct S080FB4EC_Drawable *drawable;
    u8 unk8[0x1E0 - 8];
    u16 unk1E0;
    u8 unk1E2[0x1E6 - 0x1E2];
    u16 unk1E6;
    u8 unk1E8[0x3D0 - 0x1E8];
    struct S080FB4EC_Ctx *context;
};

void AnimData_SelectPaletteBank(void *, s32);

// scope shape 2 (differences once low registers are set aside)

// confidence=low: steps the Axe palette-fade state machine in the context and selects a palette bank when one is queued.

void Axe_UpdatePaletteFade(struct S080FB4EC_Actor *a)
{
    struct S080FB4EC_Ctx *ctx;
    u8 cur;
    s32 sv;
    ctx = a->context;
    if (ctx->unkED0) {
        a->drawable->unk32 = 0x13F;
        a->drawable->unk38 = (u8 *)ctx + 0x908;
        ctx->unkECF = 0;
        ctx->unkECE = 0;
        ctx->unkED0 = 0;
    }
    if (ctx->unkED1) {
        void *drawable = a->drawable->unk0 + 0x2C;
        u16 *palette = &a->unk1E0;
        u16 *offset = &a->unk1E6;
        AnimData_SelectPaletteBank(drawable, *palette + *offset);
        ctx->unkED1 = 0;
        return;
    }
    if (a->drawable->unk32 != 0x13F)
        return;
    switch ((s8)ctx->unkECF) {
        case 0:
        case 6:
            ctx->unk90A = 0;
            break;
        case 1:
        case 5:
            ctx->unk90A = 0x10;
            break;
        case 2:
        case 4:
            ctx->unk90A = 0x18;
            break;
        case 3:
            ctx->unk90A = 0x1F;
            break;
    }
    cur = ++ctx->unkECE;
    if (cur > 9) {
        u8 raw = ctx->unkECF;
        if ((s8)ctx->unkECF > 5)
            ctx->unkECF = 0;
        else
            ctx->unkECF = raw + 1;
        ctx->unkECE = 0;
    }
}

struct AxePaletteTransition {
    u16 colors[16];
    u16 remaining, destinationBank;
    s16 red[16], green[16], blue[16];
    s16 deltaRed[16], deltaGreen[16], deltaBlue[16];
};

struct AxePaletteContext {
    u8 unk_00[0x624];
    struct AxePaletteTransition transition;
    u8 unk_708[0xE4C - 0x708];
    u32 flags;
};

// confidence=low: fades a palette bank toward another, filling per-channel deltas over duration frames

void Axe_BeginPaletteTransition(void *object, s32 destinationBank, s32 sourceBank, s32 duration)
{
    struct AxePaletteContext *context = object;
    struct AxePaletteTransition *transition = &context->transition;
    u16 **bankPointer = &gPaletteBankData;
    s32 destinationOffset = destinationBank * 32;
    u16 *palettes = *bankPointer;
    const u16 *source = &palettes[sourceBank * 16];
    const u16 *destination;
    s32 i;
    transition->destinationBank = destinationBank;
    destination = (void *)((u8 *)palettes + destinationOffset);
    i = 0;
    do {
        s32 color;
        s32 red, green, blue;
        if (transition->remaining != 256)
            color = transition->colors[i];
        else
            color = source[i];
        transition->deltaRed[i] = ((*destination & 31) - (red = color & 31)) << 8;
        transition->deltaGreen[i] = (((*destination >> 5) & 31) - (green = (color >> 5) & 31)) << 8;
        transition->deltaBlue[i] = (((*destination >> 10) & 31) - (blue = (color >> 10) & 31)) << 8;
        transition->deltaRed[i] = Div(transition->deltaRed[i], duration);
        transition->deltaGreen[i] = Div(transition->deltaGreen[i], duration);
        transition->deltaBlue[i] = Div(transition->deltaBlue[i], duration);
        transition->red[i] = red << 8;
        transition->green[i] = green << 8;
        transition->blue[i] = blue << 8;
        destination++;
        i++;
    } while (i < 16);
    transition->remaining = duration;
    {
        u32 mask = 2;
        context->flags = mask | context->flags;
    }
}

struct S080FB938;

void Axe_SetAuxiliarySpriteResource(struct ChaseActor *actor, const void *palette);

static inline s32 Axe_ChannelValue(s32 value)
{
    return value >= 0 ? value >> 8 : -(-value >> 8);
}

static inline void Axe_BindPalette(struct ChaseActor *actor, struct AxePaletteTransition *p)
{
    ((struct SceneActorPaletteSequenceView *)actor->base.drawable)->palette = p->colors;
    Axe_SetAuxiliarySpriteResource(actor, p->colors);
}

static inline u32 Axe_ReadBindingFlags(u32 *const *flags, u32 mask)
{
    return **flags & mask;
}

static inline s16 *Axe_UpdateRedSlot(s16 *const *values, s16 *const *deltas, s32 index)
{
    s16 *slot = &(*values)[index];
    *slot += (*deltas)[index];
    return slot;
}

static inline void Axe_GetPaletteUpdateHandles(s16 **deltas, u32 **flags, s16 **values,
                                               struct ChaseActor *actor,
                                               struct AxePaletteTransition *transition)
{
    *flags = &actor->base.behaviorFlags;
    *values = transition->red;
    *deltas = transition->deltaRed;
}

// confidence=low: advances the palette transition and binds its colors when the actor flags allow it.

u32 Axe_UpdatePaletteTransition(struct ChaseActor *actor)
{
    struct AxePaletteContext *context;
    struct AxePaletteTransition *p;
    s16 *red;
    s16 *deltaRed;
    s32 i;
    u32 *actorFlags;
    if (SceneActorPlacement_IsUnavailable((void *)actor))
        return 0;
    context = actor->base.context;
    if (!SceneActorChase_Test(Flags_MaskedWord(2, &context->flags), ~0))
        return 0;
    p = &context->transition;
    if (!p->remaining) {
        if (!Flags_MaskedWord(0x02008000, &actor->base.behaviorFlags))
            Axe_BindPalette(actor, p);
        return 1;
    }
    Axe_GetPaletteUpdateHandles(&deltaRed, &actorFlags, &red, actor, p);
    for (i = 0; i < 16; i++) {
        s32 r, g, b;
        s16 *redSlot;
        redSlot = Axe_UpdateRedSlot(&red, &deltaRed, i);
        p->green[i] += p->deltaGreen[i];
        p->blue[i] += p->deltaBlue[i];
        r = Axe_ChannelValue(*redSlot);
        g = Axe_ChannelValue(p->green[i]);
        b = Axe_ChannelValue(p->blue[i]);
        p->colors[i] = r | (g << 5) | (b << 10);
    }
    p->remaining--;
    if (!Axe_ReadBindingFlags(&actorFlags, 0x02008000))
        Axe_BindPalette(actor, p);
    if (!p->remaining)
        return 1;
    return 0;
}

void Axe_InitEffectPalettes(struct ChaseActor *actor)
{
    struct Axe *d = actor->base.context;
    struct SceneActorAnimationSequenceDrawable *drawable = actor->base.drawable;
    s32 i = 0;
    const u16 *src = drawable->graphics.palette;
    u16 *red = d->redPalette;
    u16 *black = d->blackPalette;
    u32 zero = 0;
    u32 full = 0x7fff;
    u16 *white = d->whitePalette;

loop: {
    u32 color;
    black[0] = zero;
    color = 0x1f;
    red[0] = color;
    color = 0x7c00;
    red[16] = color;
    white[0] = full;
    white[16] = *src++;
    red++;
    black++;
    white++;
}
    if (++i < 16)
        goto loop;
}

void Axe_SetAuxiliarySpriteResource(struct ChaseActor *actor, const void *palette)
{
    struct Axe *q = actor->base.context;
    s32 i = 3;
    const void **p = &q->trail[3].graphics.paletteData;
    do {
        *p = palette;
        p = (const void **)((u8 *)p - sizeof(struct AxeTrailSlot));
    } while (--i >= 0);
}

void Axe_SetAuxiliarySpriteRenderState(struct ChaseActor *actor, const void *palette, u32 index)
{
    struct Axe *q = actor->base.context;
    const void **a = &q->trail[0].graphics.paletteData;
    u16 *b = &q->trail[0].graphics.paletteIndex;
    s32 i = 3;
    do {
        *b = index;
        *a = palette;
        a = (const void **)((u8 *)a + sizeof(struct AxeTrailSlot));
        b = (u16 *)((u8 *)b + sizeof(struct AxeTrailSlot));
    } while (--i >= 0);
}

struct AnimatedSpriteAnimation {
    u8 unknown0[6];
    u16 priority;
    u8 unknown8[4];
    void *shared;
    u8 unknown10[0x1c];
    void *resource;
    u16 frame;
    u8 unknown32[2];
};

struct AnimatedSpriteRecord {
    u32 flags;
    u8 unknown4[0x28];
    struct AnimatedSpriteAnimation animation;
};

struct AnimatedSpriteGroup {
    u8 unknown0[0x708];
    struct AnimatedSpriteRecord sprites[4];
    u8 unknown888[0x5bc];
    u16 state;
    u16 phase;
};

struct AnimatedSpriteView {
    u32 flags;
    u8 unknown4[0x2e];
    u16 priority;
    u8 unknown34[0x2c];
};

void SpriteGroup_InitAnimatedSprites(struct ChaseActor *owner)
{
    struct AnimatedSpriteGroup *group = owner->base.context;
    u16 zero = 0;
    struct AnimatedSpriteAnimation *animation = &group->sprites[0].animation;
    struct AnimatedSpriteView *sprite = (void *)group->sprites;
    s32 offset = 0;
    s32 i = 3;
    do {
        void **sharedField;
        void **resourceField;
        void *resource;
        AnimData_Load(animation, 0x5291);
        Sprite_Init(sprite, animation, 0);
        sprite->priority = 0x13c;
        sharedField = &group->sprites[0].animation.shared;
        sharedField = (void *)((u8 *)sharedField + offset);
        *sharedField = group->unknown888;
        resource = ScriptProc_Start(0x922e, 0x3f72);
        resourceField = &group->sprites[0].animation.resource;
        *(void **)((u8 *)resourceField + offset) = resource;
        sprite->flags = 0x201;
        animation->frame = zero;
        animation++;
        animation = (void *)((u8 *)animation + 0x2c);
        sprite++;
        offset += sizeof(struct AnimatedSpriteRecord);
    } while (--i >= 0);
    {
        u16 state = 0;
        group->state = state;
    }
    group->phase = 2;
}

struct TrailSprite {
    u32 flags;
    u8 unknown4[2], angle, unknown7, scaleX, scaleY;
    u8 unknownA[6];
    u16 frame;
    u8 unknown12[0xA];
    struct ContactPosition position;
};

struct AxeTrailSpriteSlot {
    struct TrailSprite sprite;
    u8 unknown24[0x24];
    struct DisplaySequenceState sequence;
    void *commands;
    u16 remaining;
    u8 unknown5E[2];
};

struct AxeTrailContext {
    u8 unknown0[0x708];
    struct AxeTrailSpriteSlot slots[4];
    u8 unknown888[0x5BC];
    u16 nextSlot, interval;
    u8 unknownE48[0x8F];
    u8 speed;
};

static inline void CopyAngle(struct TrailSprite *src, u8 *dest)
{
    u8 angle = src->angle;
    *(u8 *)(dest + 0x70E) = angle;
}

static inline void CopyScaleX(struct TrailSprite *src, u8 *dest)
{
    u8 scaleX;
    scaleX = src->scaleX;
    *((u8 *)(dest + 0x710)) = scaleX;
}

static inline void CopyScaleY(struct TrailSprite *src, u8 *dest)
{
    u8 scaleY = src->scaleY;
    *(u8 *)(dest + 0x711) = scaleY;
}

void Axe_EmitMotionTrail(struct ChaseActor *actor)
{
    struct AxeTrailContext *trail = actor->base.context;
    u8 *context = (void *)trail;
    struct TrailSprite *sprite;
    void *sourceDrawable;
    u8 *slotBase;
    void *scaleDrawable;
    u8 *scaleSlotBase;
    if ((*((u32 *)actor->base.drawable)) & 1)
        return;
    {
        s32 zero = Mod(actor->base.frameCounter, trail->interval);
        if (zero)
            return;
        {
            struct SceneActorControlMotionDrawable *drawable = actor->base.drawable;
            struct DisplaySequenceState *source = &drawable->sequence;
            u16 sequence = source->sequenceId;
            {
                u32 offset = trail->nextSlot * sizeof(struct AxeTrailSpriteSlot);
                u8 *destination = context + (offset + 0x708);
                u8 *sequenceState = destination + 0x48;
                u32 sequenceOffset = 0x760;
                void *commands = *(void **)(context + sequenceOffset + offset);
                u8 speed = trail->speed;
                Anim_SetSequence((void *)sequenceState, commands, sequence, speed, zero);
            }
        }
    }
    sprite = (void *)((trail->nextSlot * sizeof(struct AxeTrailSpriteSlot)) + (u32)context);
    sprite = (void *)((u8 *)sprite + 0x708);
    PlayerCue_StepSequence((void *)sprite, &trail->slots[trail->nextSlot].sequence);
    {
        u32 offset =
            ((struct AxeTrailContext *)context)->nextSlot * sizeof(struct AxeTrailSpriteSlot);
        u8 *slot = context + offset;
        *((struct ContactPosition *)(slot + 0x724)) = actor->base.body.position;
    }
    {
        u32 offset =
            ((struct AxeTrailContext *)context)->nextSlot * sizeof(struct AxeTrailSpriteSlot);
        u8 *slots = context + 0x708;
        *((u32 *)(slots + offset)) &= ~1;
        slots += ((struct AxeTrailContext *)context)->nextSlot * sizeof(struct AxeTrailSpriteSlot);
        *((u32 *)slots) |= 2;
    }
    {
        u32 offset =
            ((struct AxeTrailContext *)context)->nextSlot * sizeof(struct AxeTrailSpriteSlot);
        u8 *slot = context + offset;
        *((u16 *)(slot + 0x764)) = 8;
    }
    sourceDrawable = actor->base.drawable;
    slotBase = context + ((((struct AxeTrailContext *)context)->nextSlot) *
                          sizeof(struct AxeTrailSpriteSlot));
    CopyAngle(sourceDrawable, slotBase);
    scaleDrawable = actor->base.drawable;
    scaleSlotBase = context + ((((struct AxeTrailContext *)context)->nextSlot) *
                               sizeof(struct AxeTrailSpriteSlot));
    CopyScaleX(scaleDrawable, scaleSlotBase);
    sourceDrawable = actor->base.drawable;
    slotBase = context + ((((struct AxeTrailContext *)context)->nextSlot) *
                          sizeof(struct AxeTrailSpriteSlot));
    CopyScaleY(sourceDrawable, slotBase);
    if ((++(((struct AxeTrailContext *)context)->nextSlot)) > 3)
        ((struct AxeTrailContext *)context)->nextSlot = 0;
}

static inline const void *Axe_TrailResource(struct Axe *context, u32 index)
{
    return context->trail[index].resource;
}

static inline void Axe_CopyTrailScaleX(struct DisplaySequenceSprite *source, struct Axe *row)
{
    row->trail[0].sprite.palette = source->palette;
}

static inline void Axe_CopyTrailScaleY(struct DisplaySequenceSprite *source, struct Axe *row)
{
    row->trail[0].sprite.paletteVariant = source->paletteVariant;
}

static inline u32 Axe_StepSequence(struct DisplaySequenceSprite *owner,
                                   struct DisplaySequenceState *state)
{
    const u16 *command;
    u32 ended;
    {
        u32 offset = DisplaySequence_GetIndex(state) << 1;
        const u16 *table;
        table = state->commands;
        command = (const u16 *)((u32)table + offset);
    }
    owner->frame = *command >> 6;
    {
        union DisplaySequenceControl control;
        u32 bit, first;
        control.raw = state->flags;
        bit = 1;
        first = (u8)(control.raw & bit);
        if (first != (((*command & 0x30) >> 4) & bit))
            owner->flags |= 4;
        else
            owner->flags &= ~4;
    }
    {
        union DisplaySequenceControl control;
        u32 bit, first;
        control.raw = state->flags;
        bit = 2;
        first = (u8)(control.raw & bit);
        if (first != (((*command & 0x30) >> 4) & bit))
            owner->flags |= 8;
        else
            owner->flags &= ~8;
    }
    if (++state->elapsed >= state->duration) {
        state->elapsed = 0;
        if (state->flags & 4) {
            if (state->index == 0) {
                if (state->flags & 8)
                    state->elapsed = state->duration;
                else
                    state->index = state->frameCount - 1;
                ended = 1;
            } else {
                state->index--;
                ended = 0;
            }
        } else {
            if (++state->index >= state->frameCount) {
                if (state->flags & 8) {
                    state->elapsed = state->duration;
                    state->index = state->frameCount - 1;
                } else {
                    state->index = 0;
                }
                ended = 1;
            } else {
                ended = 0;
            }
        }
        command = state->commands + state->index;
        state->frameWeight = DisplaySequence_MaskCommand(DisplaySequence_ReadCommand(command), 15);
        state->duration = (state->frameWeight * (s32)state->speed) >> 6;
        if (state->duration == 0)
            state->duration = 1;
    } else {
        ended = 0;
    }
    return ended;
}

void ActorPhase_EmitTrailSprite(struct ChaseActor *actor, const struct ContactPosition *position,
                                u32 angle)
{
    struct Axe *context = actor->base.context;
    struct DisplaySequenceSprite *sprite;
    struct DisplaySequenceState *state;
    u32 spriteOffset;
    if (((struct DisplaySequenceSprite *)actor->base.drawable)->flags & 1)
        return;
    {
        s32 zero = Mod(actor->base.frameCounter, context->trailSpacing);
        if (zero)
            return;
        {
            struct DisplaySequenceState *source =
                &((struct SceneActorControlMotionDrawable *)actor->base.drawable)->sequence;
            u16 selection = source->sequenceId;
            u32 indexOffset = context->nextTrail * sizeof(struct AxeTrailSlot);
            struct AxeTrailSlot *slot;
            struct DisplaySequenceState *destination;
            const void *resource;
            spriteOffset = (u32) & ((struct Axe *)0)->trail[0];
            slot = (struct AxeTrailSlot *)((u8 *)context + (indexOffset + spriteOffset));
            destination = &slot->sequence;
            resource = Axe_TrailResource(context, context->nextTrail);
            Anim_SetSequence(destination, resource, selection, context->variant, zero);
        }
    }
    sprite = (struct DisplaySequenceSprite *)(context->nextTrail * sizeof(struct AxeTrailSlot) +
                                              (u32)context);
    sprite = (struct DisplaySequenceSprite *)((u8 *)sprite + spriteOffset);
    state = (struct DisplaySequenceState *)(context->nextTrail * sizeof(struct AxeTrailSlot) +
                                            (u32)context);
    state =
        (struct DisplaySequenceState *)((u8 *)state + (u32) & ((struct Axe *)0)->trail[0].sequence);
    Axe_StepSequence(sprite, state);
    context->trail[context->nextTrail].sprite.position = *position;
    context->trail[context->nextTrail].sprite.flags &= ~1;
    context->trail[context->nextTrail].sprite.flags |= 2;
    context->trail[context->nextTrail].lifetime = 8;
    context->trail[context->nextTrail].sprite.attribute = angle;
    {
        struct DisplaySequenceSprite *source = actor->base.drawable;
        struct Axe *row =
            (void *)((u8 *)context + context->nextTrail * sizeof(struct AxeTrailSlot));
        Axe_CopyTrailScaleX(source, row);
    }
    {
        struct DisplaySequenceSprite *source = actor->base.drawable;
        struct Axe *row =
            (void *)((u8 *)context + context->nextTrail * sizeof(struct AxeTrailSlot));
        Axe_CopyTrailScaleY(source, row);
    }
    if (++context->nextTrail > 3)
        context->nextTrail = 0;
}

void Axe_TickAuxiliarySpriteTimers(struct ChaseActor *actor)
{
    u8 *q = actor->base.context;
    u32 *flags = &((struct Axe *)q)->trail[0].sprite.flags;
    u8 *p = q;
    u32 off = (u32) & ((struct Axe *)0)->trail[0].lifetime;
    s32 i = 3;
    u32 one = 1;
    do {
        u16 *timer = (u16 *)(p + off);
        if (*timer != 0) {
            if (--*timer == 0) {
                {
                    u32 f = *flags;
                    one = 1;
                    *flags = f | one;
                }
            }
        }
        flags = (u32 *)((u8 *)flags + sizeof(struct AxeTrailSlot));
        p += sizeof(struct AxeTrailSlot);
    } while (--i >= 0);
}

void Sprite_Remove(void *);

void Axe_RemoveAuxiliarySprites(struct ChaseActor *actor)
{
    struct AxeTrailSlot *p = ((struct Axe *)actor->base.context)->trail;
    s32 i = 3;
    do {
        if (p->sprite.unknown4[0])
            Sprite_Remove(&p->sprite);
        p++;
    } while (--i >= 0);
}

// Remaining: route index byte uses r3 instead of r2.

#define gRandomTableIndex gRandomIndex

struct RoutePointSelection {
    u16 id;
    u8 index, mode;
    u8 *data;
    u16 *entry;
};

struct RouteSelectionPosition {
    s32 x : 16;
    s32 y : 16;
    s32 z : 16;
    s32 pad : 16;
};

extern u32 gRandomTableIndex;

void Axe_RecordRoutePoint(struct ChaseActor *actor, u32 x);

u8 Axe_IsRoutePointRecent(struct ChaseActor *, s32);

static inline u32 Random(void)
{
    gRandomTableIndex = (gRandomTableIndex + 1) & 1023;
    {
        u32 offset = gRandomTableIndex * 2;
        const u16 *table = gRandomTable;
        return *(u16 *)(offset + (u32)table);
    }
}

inline static u32 Random_SelectDistantWaypoint(void)
{
    gRandomIndex = (gRandomIndex + 1) & 1023;
    {
        u32 offset = gRandomIndex * 2;
        const u16 *table = *((typeof(gRandomTable) *)0x0203B400);
        return *((u16 *)(offset + ((u32)table)));
    }
}

void ActorPhase_SelectDistantWaypoint(struct ChaseActor *actor, struct ContactPosition *out,
                                      struct RouteSelectionPosition *origin, s32 limit)
{
    struct RoutePointSelection route;
    struct RoutePointSelection point;
    struct RouteSelectionPosition delta;
    s32 bestDistance;
    s32 i;
    s32 distance;
    unsigned long long wide;
    u16 *entry;
    s32 bestIndex;
    Route_Start((void *)(&route), ((struct Axe *)actor->base.context)->routeId, 0, 0);
    bestDistance = 0;
    bestIndex = -1;
    for (i = 0; i < (*route.data); i++) {
        point.entry = (u16 *)(Path_GetPointData(route.data) + (i * 8));
        delta = *origin;
        delta.x = point.entry[0] - (*((u16 *)(&delta)));
        {
            u16 z = point.entry[1] - (*(((u16 *)(&delta)) + 2));
            *(((u32 *)(&delta)) + 1) = ((*(((u32 *)(&delta)) + 1)) & 0xffff0000) | z;
        }
        distance = ((*((s16 *)(&delta))) * (*((s16 *)(&delta)))) +
                   ((*(((s16 *)(&delta)) + 2)) * (*(((s16 *)(&delta)) + 2)));
        if (((distance <= limit) && (bestDistance < distance)) &&
            (!Axe_IsRoutePointRecent(actor, i))) {
            bestIndex = i;
            bestDistance = distance;
        }
    }

    if (!bestDistance)
        bestIndex = Mod(Random_SelectDistantWaypoint(), *route.data);
    Axe_RecordRoutePoint(actor, bestIndex);
    {
        struct RoutePointSelection *dst = &route;
        u32 mode = 0;
        dst->index = bestIndex;
        dst->mode = mode;
    }
    route.entry = (u16 *)Path_GetPointData(route.data);
    route.entry = (u16 *)(((u8 *)route.entry) + (route.index * 8));
    wide = route.entry[0];
    out->x = wide;
    out->y = actor->base.groundHeight;
    entry = route.entry;
    out->z = entry[1];
}

static inline s32 Burst080FC094_Scale(s32 value)
{
    s32 result;
    if (value >= 0)
        result = value >> 12;
    else
        result = -(-value >> 12);
    return result;
}

// confidence=low: spawns a particle burst at a point offset from the drawable by its heading.

void Axe_SpawnHeadingBurst(struct ChaseActor *actor, s32 count)
{
    struct DisplaySequenceSprite *drawable = actor->base.drawable;
    struct ContactPosition range;
    struct ContactPosition offset;
    struct ContactPosition velocity;
    struct ContactPosition position;
    s32 x, z, x2, z2;

    range.x = 120;
    range.y = 60;
    range.z = 120;
    position = drawable->position;
    ZeroFill8(&offset);
    ZeroFill8(&velocity);
    offset.x = -140;
    offset.z = -140;
    x = Burst080FC094_Scale(offset.x * gUnk_086149C4[(drawable->attribute + 64) & 255]);
    z = Burst080FC094_Scale(offset.z * gUnk_086149C4[drawable->attribute]);
    position.x = x + z + position.x;
    x2 = Burst080FC094_Scale(-(offset.x * gUnk_086149C4[drawable->attribute]));
    z2 = Burst080FC094_Scale(offset.z * gUnk_086149C4[(drawable->attribute + 64) & 255]);
    position.z = x2 + z2 + position.z;
    SceneParticleBurst_CreateWithPause(&position, &velocity, &range, count, 2, 4);
}

static inline s32 Scale_SpawnSparks(s32 x)
{
    s32 result;
    if (x >= 0)
        result = x >> 12;
    else
        result = -(-x >> 12);
    return result;
}

void Axe_SpawnSparks(struct ChaseActor *actor, struct ContactPosition *origin, s32 angle, s32 frame)
{
    struct {
        u32 zero;
        u32 values[0];
    } buf;
    struct ContactPackedVector range;
    struct ContactPosition offset;
    struct ContactPosition position;
    struct Axe *context = actor->base.context;
    if (Mod(frame, 2) == 0) {
        s32 x, z, x2;
        range.x = 60;
        range.y = 60;
        range.z = 40;
        position = *origin;
        MonsterInit_ClearPosition(&buf.zero, &offset);
        switch (context->sparkStep) {
            case 0:
                offset.x = -120;
                offset.z = -120;
                context->sparkStep++;
                break;
            case 1:
                offset.x = -75;
                offset.z = -75;
                context->sparkStep++;
                break;
            case 2:
                offset.x = -30;
                offset.z = -30;
                context->sparkStep++;
                break;
            case 3:
                offset.x = 15;
                offset.z = 15;
                context->sparkStep++;
                break;
            case 4:
                offset.x = 60;
                offset.z = 60;
                context->sparkStep = 0;
                break;
        }
        x = Scale_SpawnSparks(offset.x * gUnk_086149C4[(angle + 64) & 255]);
        z = Scale_SpawnSparks(offset.z * gUnk_086149C4[angle & 255]);
        position.x = x + z + position.x;
        x2 = Scale_SpawnSparks(-(offset.x * gUnk_086149C4[angle & 255]));
        z = offset.z * gUnk_086149C4[(angle + 64) & 255];
        if (z >= 0)
            z >>= 12;
        else
            z = -(-z >> 12);
        position.z = x2 + z + position.z;
        SparkParticle_Spawn(&position, 2, &range, 0, 0, 22, 2, 1);
    }
}

void Camera_RequestShake(u8);

void Axe_UpdateScreenShake(struct ChaseActor *actor)
{
    struct Axe *q = actor->base.context;
    u32 m = 0x40;
    if (!(actor->base.motionFlags & m)) {
        u8 *p = &q->shakeFrames;
        if (*p) {
            Camera_RequestShake(q->parameterEB2);
            *p = *p - 1;
        }
    }
}

extern const s16 gUnk_08606458[];

extern const s16 gUnk_08606460[];

u8 ActorPhase_UpdateReverseFlickerTimer(u32 *flags, s32 timer, s32 low, s32 high)
{
    u32 index = 2;
    if (timer <= high) {
        index = 0;
        if (timer > low)
            index = 1;
    }
    if (Mod(timer, gUnk_08606458[index]) == 0) {
        if (*flags & 1)
            *flags &= ~1;
        else
            *flags |= 1;
    }
    if (timer > 60) {
        *flags |= 1;
        return 1;
    }
    return 0;
}

u8 ActorPhase_UpdateFlicker(u32 *flags, s32 timer, s32 low, s32 high)
{
    u32 index = 2;
    if (timer <= high) {
        index = 0;
        if (timer > low)
            index = 1;
    }
    if (Mod(timer, gUnk_08606460[index]) == 0) {
        if (*flags & 1)
            *flags &= ~1;
        else
            *flags |= 1;
    }
    if (timer > 60) {
        *flags |= 1;
        return 1;
    }
    return 0;
}

void Collider_SetBodyProperties(void *, u16, u32, u32);

void Collider_SetAttackProperties(void *, u32, u32, u32, u32, u32);

void Axe_LatchColliderContact(void);

void Axe_OnSecondaryColliderContact(void);

u32 Shadow_Initialize(void *, void *, s32);

static inline u16 Axe_SequenceSelection(struct DisplaySequenceState *source)
{
    return source->sequenceId;
}

void Axe_InitAttackTrail(struct ChaseActor *s)
{
    struct ContactPackedVector bodySize;
    struct ContactPackedVector bodyOffset;
    struct ContactPackedVector attackSize;
    struct ContactPackedVector attackOffset;
    struct Axe *d = s->base.context;
    void *resource = &d->auxiliaryGraphics;
    struct DisplaySequenceSprite *sprite;
    struct DisplaySequenceState *frames;
    void *sequence;
    s32 i;
    AnimData_Load(resource, 0x5291);
    sprite = &d->auxiliarySprite;
    Sprite_Init(sprite, resource, 0);
    sequence = ScriptProc_Start(0x922e, 0x3f72);
    {
        struct DisplaySequenceState *drawable =
            &((struct SceneActorControlMotionDrawable *)s->base.drawable)->sequence;
        u32 index = Axe_SequenceSelection(drawable);
        frames = &d->auxiliarySequence;
        Anim_SetSequence(frames, sequence, index, d->variant, 0);
    }
    Axe_StepSequence(sprite, frames);
    d->auxiliarySprite.flags |= 1;
    d->auxiliarySprite.position = s->base.body.position;
    d->auxiliarySprite.priority = 1;
    bodySize.x = 24;
    bodySize.y = 70;
    bodySize.z = 24;
    bodyOffset.x = 0;
    bodyOffset.y = 0;
    bodyOffset.z = 0;
    for (i = 0; i < 3; i++) {
        void *c = &d->body[i];
        Collider_Init(c, 0, 0x5001, 0, 16, &bodySize, &bodyOffset);
        Collider_SetBodyProperties(c, 0, 0, 0);
        Collider_SetPosition(c, &d->origin, 0);
        Collider_SetCallback(c, Axe_LatchColliderContact, d);
        Collider_Refresh(c);
    }

    attackSize.x = 24;
    attackSize.y = 64;
    attackSize.z = 24;
    attackOffset.x = 0;
    attackOffset.y = 0;
    attackOffset.z = 0;
    for (i = 0; i < 3; i++) {
        void *c = &d->attack[i];
        Collider_Init(c, 0, 0x2401, 0, 16, &attackSize, &attackOffset);
        {
            u16 first = s->base.attacks.first[0];
            u16 second = s->base.attacks.second[0];
            u32 flags = s->base.attacks.flags[0];
            u16 parameter = s->base.attacks.parameter[0];
            Collider_SetAttackProperties(c, first, second, 0, flags, parameter);
        }
        Collider_SetPosition(c, &d->origin, 0);
        Collider_SetCallback(c, Axe_OnSecondaryColliderContact, d);
    }

    d->shadowActive = Shadow_Initialize(d->auxiliaryShadow, &d->auxiliarySprite.position, 0);
    ActorShadow_Disable(d->auxiliaryShadow);
}

void Collider_SetPosition(void *, void *, u32);

s32 Collider_Refresh(void *);

void Axe_UpdateAttackColliderPositions(struct ChaseActor *actor)
{
    struct Axe *q = actor->base.context;
    s32 i = 0;
    struct ContactPosition *k = &q->auxiliarySprite.position;
    for (; i < 3; i++) {
        struct ContactCollider *e;
        Collider_SetPosition(&q->body[i], k, 0);
        e = &q->attack[i];
        Collider_SetPosition(e, k, 0);
        Collider_Refresh(e);
    }
}

void Collider_Remove(void *);

void Shadow_Destroy(u8 *);

void Axe_ReleaseAuxiliarySprite(struct ChaseActor *actor)
{
    struct Axe *q = actor->base.context;
    if (q->auxiliarySprite.unknown4[0]) {
        struct ContactCollider *p = q->body;
        s32 i = 2;
        do {
            Collider_Remove(p);
            p++;
        } while (--i >= 0);
        Shadow_Destroy(q->auxiliaryShadow);
        Sprite_Remove(&q->auxiliarySprite);
    }
}

void Axe_CopyAuxiliaryVector(struct ChaseActor *s, const struct ContactPosition *src)
{
    struct Axe *trail = s->base.context;
    trail->auxiliarySprite.position = *src;
}

struct S080FC8B0 {
    u8 filler[0x3d0];
    u8 *q;
};

void Axe_SetAuxiliarySpriteParameters(struct S080FC8B0 *s, u32 a, u32 b)
{
    u8 *q = s->q;
    *(u16 *)(q + 0xa6a) = b;
    *(u32 *)(q + 0xa70) = a;
}

void ActorCollision_InitThreeAuxiliaryAttacks(struct ChaseActor *actor)
{
    struct Axe *group = actor->base.context;
    struct ContactPackedVector size, offset;
    s32 i;
    size.x = 24;
    size.y = 64;
    size.z = 24;
    offset.x = 0;
    offset.y = 0;
    offset.z = 0;
    for (i = 0; i < 3; i++) {
        struct ContactCollider *collider = &group->auxiliaryAttack[i];
        Collider_Init(collider, actor->base.body.id, 0x2401, 0, 16, &size, &offset);
        Collider_SetAttackProperties(collider, SceneActorCollision_ReadAttackField3C(actor, 3),
                                     SceneActorCollision_ReadAttackField40(actor, 3), 0,
                                     SceneActorCollision_ReadAttackReactionFlags(actor, 3),
                                     SceneActorCollision_ReadAttackField44(actor, 3));
        Collider_SetPosition(collider, &actor->base.body.position, 0);
        Collider_SetCallback(collider, Axe_OnAttackColliderContact, actor);
    }
}

static inline s32 Scale_UpdateTrailColliders(s32 x)
{
    s32 result;
    if (x >= 0)
        result = x >> 12;
    else
        result = -(-x >> 12);
    return result;
}

void Axe_UpdateTrailColliders(u8 *actor)
{
    u8 *ctx = *(u8 **)(actor + 0x3d0);
    u8 *entry;
    u32 idx;
    s32 i;
    struct {
        u32 zero;
        u32 values[0];
    } buf;
    struct ContactPosition offset;
    struct ContactPosition position;
    s32 x, z;
    u16 *ip = (u16 *)(ctx + 0xe44);
    if (*ip == 0)
        idx = 3;
    else
        idx = *ip - 1;
    entry = ctx + (idx * 0x60 + 0x708);
    for (i = 0; i < 3; i++) {
        MonsterInit_ClearPosition(&buf.zero, &offset);
        position = *(struct ContactPosition *)(ctx + 0xa24);
        switch (i) {
            case 0:
                offset.x = -120;
                offset.z = -120;
                break;
            case 1:
                offset.x = -30;
                offset.z = -30;
                break;
            case 2:
                offset.x = 60;
                offset.z = 60;
                break;
        }
        x = Scale_UpdateTrailColliders(offset.x * gUnk_086149C4[(entry[6] + 64) & 255]);
        z = Scale_UpdateTrailColliders(offset.z * gUnk_086149C4[entry[6]]);
        position.x = x + z + position.x;
        x = Scale_UpdateTrailColliders(-(offset.x * gUnk_086149C4[entry[6]]));
        z = Scale_UpdateTrailColliders(offset.z * gUnk_086149C4[(entry[6] + 64) & 255]);
        position.z = x + z + position.z;
        Collider_SetPosition(ctx + 0x928 + i * 0x54, &position, 0);
        Collider_Refresh(ctx + 0x928 + i * 0x54);
    }
}

void Axe_RemoveSecondaryColliders(struct ChaseActor *actor)
{
    struct ContactCollider *p = ((struct Axe *)actor->base.context)->extraBody;
    s32 i = 1;
    do {
        Collider_Remove(p);
        p++;
    } while (--i >= 0);
}

static inline s32 Scale_UpdateBodyColliders(s32 value)
{
    s32 result;
    if (value >= 0)
        result = value >> 12;
    else
        result = -(-value >> 12);
    return result;
}

void Axe_UpdateBodyColliders(struct ChaseActor *actor)
{
    struct Axe *context = actor->base.context;
    struct DisplaySequenceSprite *drawable = actor->base.drawable;
    s32 index = 0;
    struct ContactCollider *attack = &actor->base.colliderB;
    struct {
        u32 zero;
        u32 values[0];
    } buf;
    struct ContactPosition offset;
    struct ContactPosition position;
    s32 i, x, z;

    for (index = 0; index < 3; index++) {
        if (WordFlags_Has(0x40, &context->flagsE4C) ||
            SceneActorChase_HasPlacementFlags(actor, 0x800)) {
            ContactCollider_SetFlags(attack, 4);
            ContactCollider_SetFlags(&actor->base.colliderA, 4);
            for (i = 0; i < 2; i++) {
                ContactCollider_SetFlags(&context->extraBody[i], 4);
                ContactCollider_SetFlags(&context->extraAttack[i], 4);
            }
            break;
        }
        ContactCollider_ClearFlags(attack, 4);
        ContactCollider_ClearFlags(&actor->base.colliderA, 4);
        for (i = 0; i < 2; i++) {
            ContactCollider_ClearFlags(&context->extraBody[i], 4);
            ContactCollider_ClearFlags(&context->extraAttack[i], 4);
        }
        MonsterInit_ClearPosition(&buf.zero, &offset);
        position = actor->base.body.position;
        switch (index) {
            case 0:
                offset.x = -120;
                offset.z = -120;
                break;
            case 1:
                offset.x = -30;
                offset.z = -30;
                break;
            case 2:
                offset.x = 60;
                offset.z = 60;
                break;
        }
        x = Scale_UpdateBodyColliders(offset.x * gUnk_086149C4[(drawable->attribute + 64) & 255]);
        z = Scale_UpdateBodyColliders(offset.z * gUnk_086149C4[drawable->attribute]);
        position.x = x + z + position.x;
        x = Scale_UpdateBodyColliders(-(offset.x * gUnk_086149C4[drawable->attribute]));
        z = Scale_UpdateBodyColliders(offset.z * gUnk_086149C4[(drawable->attribute + 64) & 255]);
        position.z = x + z + position.z;
        if (index == 0) {
            Collider_SetPosition(&actor->base.colliderA, &position, 0);
            Collider_SetPosition(attack, &position, 0);
            Collider_Refresh(attack);
        } else {
            Collider_SetPosition(&context->extraBody[index - 1], &position, 0);
            Collider_SetPosition(&context->extraAttack[index - 1], &position, 0);
            Collider_Refresh(&context->extraAttack[index - 1]);
        }
    }
}

static inline u8 Axe_TestMaskedFlags(u32 value)
{
    if (value)
        return 1;
    return 0;
}

struct AxeAttackSelection {
    u32 *flags;
};

static inline u8 Axe_TestSelectionFlags(u32 mask, const struct AxeAttackSelection *selection)
{
    return WordFlags_Has(mask, selection->flags);
}

#define Axe_ApplyAttack(collider, actor, index)                                                    \
    do {                                                                                           \
        u16 power = (actor)->base.attacks.first[index];                                            \
        u16 force = (actor)->base.attacks.second[index];                                           \
        u32 flags = (actor)->base.attacks.flags[index];                                            \
        u16 duration = (actor)->base.attacks.parameter[index];                                     \
        Collider_SetAttackProperties(collider, power, force, 0, flags, duration);                  \
    } while (0)

// confidence=low: fires the axe's attack on the actor collider and both attack colliders, with attack parameters chosen by the context's flags.

void Axe_FireAttack(struct ChaseActor *actor)
{
    struct Axe *context = actor->base.context;
    struct AxeAttackSelection selection;

    if (WordFlags_Has(0x200, &context->flagsE4C)) {
        Axe_ApplyAttack(&actor->base.colliderB, actor, 2);
        Axe_ApplyAttack(&context->extraAttack[0], actor, 2);
        Axe_ApplyAttack(&context->extraAttack[1], actor, 2);
    } else {
        u32 value = Flags_MaskedWord(0x800, &context->flagsE4C);
        selection.flags = &context->flagsE4C;
        if (Axe_TestMaskedFlags(value)) {
            s32 power = actor->base.attacks.first[2] * 2;
            s32 force = actor->base.attacks.second[2] * 2;
            {
                u32 reactionFlags = actor->base.attacks.flags[2];
                u16 duration = actor->base.attacks.parameter[2];
                Collider_SetAttackProperties(&actor->base.colliderB, power, force, 0, reactionFlags,
                                             duration);
            }
            {
                u32 reactionFlags = actor->base.attacks.flags[2];
                u16 duration = actor->base.attacks.parameter[2];
                Collider_SetAttackProperties(&context->extraAttack[0], power, force, 0,
                                             reactionFlags, duration);
            }
            {
                u32 reactionFlags = actor->base.attacks.flags[2];
                u16 duration = actor->base.attacks.parameter[2];
                Collider_SetAttackProperties(&context->extraAttack[1], power, force, 0,
                                             reactionFlags, duration);
            }
        } else if (Axe_TestSelectionFlags(0x400, &selection)) {
            Axe_ApplyAttack(&actor->base.colliderB, actor, 1);
            Axe_ApplyAttack(&context->extraAttack[0], actor, 1);
            Axe_ApplyAttack(&context->extraAttack[1], actor, 1);
        } else if (Axe_TestSelectionFlags(0x1000, &selection)) {
            Axe_ApplyAttack(&actor->base.colliderB, actor, 0);
            Axe_ApplyAttack(&context->extraAttack[0], actor, 0);
            Axe_ApplyAttack(&context->extraAttack[1], actor, 0);
        } else {
            Axe_ApplyAttack(&actor->base.colliderB, actor, 0);
            Axe_ApplyAttack(&context->extraAttack[0], actor, 0);
            Axe_ApplyAttack(&context->extraAttack[1], actor, 0);
        }
    }
}

struct ActorGroupOptions {
    u8 unknown0[0xe3c];
    u8 option0, option1, option2, option3, option4, option5;
};

static inline s32 ReadOption(s32 defaultValue)
{
    s32 value;
    if (Script_GetPc())
        value = Script_GetValue();
    else
        value = defaultValue;
    return value;
}

void ActorGroup_ReadScriptOptions(struct ChaseActor *actor)
{
    struct ActorGroupOptions *group = actor->base.context;
    if (Script_SeekToKeyword('O')) {
        group->option0 = ReadOption(6);
        group->option1 = ReadOption(12);
        group->option2 = ReadOption(60);
        group->option3 = ReadOption(3);
        group->option4 = ReadOption(60);
        group->option5 = ReadOption(30);
    }
}

void SceneActorControl_UpdateHoverHeight(struct ChaseActor *actor);

void Axe_UpdateRouteWander(struct ChaseActor *actor);

void Axe_UpdateAttackFlashFade(struct ChaseActor *actor);

s32 Axe_PrepareUpdate(void *p)
{
    SceneActorControl_UpdateHoverHeight(p);
    Axe_UpdateRouteWander(p);
    Axe_UpdateAttackFlashFade(p);
    return 1;
}

void Axe_UpdateScreenShake(struct ChaseActor *actor);

void Axe_TickAuxiliarySpriteTimers(struct ChaseActor *actor);

struct ChaseActor;

u32 Axe_UpdatePaletteTransition(struct ChaseActor *actor);

void Axe_UpdateBodyColliders(struct ChaseActor *actor);

void Axe_TickCooldowns(struct ChaseActor *actor);

void Axe_FireAttack(struct ChaseActor *actor);

s32 Axe_UpdateEffects(void *s)
{
    Axe_UpdateScreenShake(s);
    Axe_TickAuxiliarySpriteTimers(s);
    Axe_UpdatePaletteTransition(s);
    Axe_UpdateBodyColliders(s);
    Axe_TickCooldowns(s);
    Axe_FireAttack(s);
    return 1;
}

void Axe_UpdateFadeOut(struct ChaseActor *actor, s32 frame);

void Axe_UpdateFadeOutPhase(struct ChaseActor *actor)
{
    struct Axe *group = actor->base.context;
    if (Chase_TakeArrived(actor)) {
        SceneActorChase_SetControl(actor, Axe_UpdateFadeOut, 0x22);
        {
            u32 flag = 0x100;
            flag |= actor->base.motionFlags;
            actor->base.motionFlags = flag;
        }
        Axe_SetAuxiliarySpriteRenderState(actor, group->blackPalette, 0x13C);
        {
            u32 flag = 0x400;
            group->flagsE4C |= flag;
        }
    }
    Axe_EmitMotionTrail(actor);
    if (actor->base.stepDone) {
        SceneActorChase_SetControl(actor, ActorPhase_UpdateSpriteCollapse, 0x23);
        Chase_SetState(actor, 0, 2, 1);
    } else {
        actor->base.counter2AC++;
    }
}

// foundry draft (worker best 92.0%); what differs is mostly register allocation and instruction order

static inline void ResetPhase(struct ChaseActor *actor, u32 state, u32 phase, u32 behavior,
                              u32 counter)
{
    actor->base.state2A8 = state;
    actor->base.phase2A9 = phase;
    actor->base.behavior2AA = behavior;
    actor->base.counter2AC = counter;
    actor->base.field2B1 = 1;
}

void ActorGroup_ResetActivePhase(struct ChaseActor *actor)
{
    struct Axe *group = actor->base.context;
    if (actor->base.field2B1)
        actor->base.field2B1 = 0;
    if (actor->base.stepDone) {
        u32 z3, z4;
        {
            u32 m;
            u16 *f = &actor->base.motionFlags;
            m = ~0x100;
            actor->base.motionFlags = m & *f;
            z3 = 0;
            z4 = 0;
        }
        {
            u32 *g = &group->flagsE4C;
            u32 m = ~0x400;
            *g &= m;
        }
        ResetPhase(actor, z3, z3, 3, z4);
    } else {
        actor->base.counter2AC++;
    }
}

void SceneActorControl_TiltRotation(struct ChaseActor *actor, s32 elapsed);

void ActorGroup_BeginMotionPhase(struct ChaseActor *actor)
{
    struct Axe *group = actor->base.context;
    if (Chase_TakeArrived(actor)) {
        SceneActorChase_SetControl(actor, SceneActorControl_TiltRotation, 0x31);
        {
            u32 flag = 0x100;
            flag |= actor->base.motionFlags;
            actor->base.motionFlags = flag;
        }
        {
            u32 flag = 0x200;
            group->flagsE4C |= flag;
        }
        Axe_SetAuxiliarySpriteRenderState(actor, group->blackPalette, 0x13c);
        group->attackCooldowns[0] = group->attackDelays[0];
    }
    if (actor->base.stepDone) {
        SceneActorChase_SetControl(actor, ActorPhase_UpdateSpriteCollapse, 0x23);
        Chase_SetState(actor, 0, 2, 3);
    } else {
        actor->base.counter2AC++;
    }
}

void Axe_UpdateRecoverTimer(struct ChaseActor *actor)
{
    if (actor->base.field2B1)
        actor->base.field2B1 = 0;
    if (actor->base.stepDone) {
        u32 mask = 0x100;
        actor->base.motionFlags = actor->base.motionFlags & ~mask;
        Chase_SetState(actor, 0, 0, 3);
    } else
        actor->base.counter2AC++;
}

void Axe_UpdateTargetWindup(struct ChaseActor *actor, s32 elapsed);

static inline void Or16(u32 m, u16 *p)
{
    *p = m | *p;
}

static inline void Restart(struct ChaseActor *actor)
{
    SceneActorChase_SetControl(actor, ActorPhase_UpdateSpriteCollapse, 0x23);
    Chase_SetState(actor, 0, 2, 5);
}

void SceneActorControl_UpdateControl35Phase(struct ChaseActor *actor)
{
    struct Axe *context = actor->base.context;
    if (SceneActorControl_TakeFlag(&actor->base.field2B1)) {
        SceneActorChase_SetControl(actor, Axe_UpdateTargetWindup, 0x35);
        Or16(0x100, &actor->base.motionFlags);
        WordFlags_Set(0x800, &context->flagsE4C);
        context->attackCooldowns[1] = context->attackDelays[1];
        Or16(0x1000, &actor->base.placementFlags);
    }
    if (actor->base.stepDone) {
        Restart(actor);
    } else {
        ++actor->base.counter2AC;
    }
}

void Axe_UpdateRecoverTimer2(struct ChaseActor *actor)
{
    if (actor->base.field2B1)
        actor->base.field2B1 = 0;
    if (actor->base.stepDone) {
        u32 mask = 0x100;
        actor->base.motionFlags = actor->base.motionFlags & ~mask;
        Chase_SetState(actor, 0, 0, 3);
    } else
        actor->base.counter2AC++;
}

void Axe_UpdateTrailFadeIn(struct ChaseActor *actor, s32 frame);

void ActorGroup_BeginExtendedMotion(struct ChaseActor *actor)
{
    struct Axe *group = actor->base.context;
    if (Chase_TakeArrived(actor)) {
        SceneActorChase_SetControl(actor, Axe_UpdateTrailFadeIn, 0x27);
        {
            u32 flag = 0x100;
            flag |= actor->base.motionFlags;
            actor->base.motionFlags = flag;
        }
        {
            u32 flag = 0x1000;
            group->flagsE4C |= flag;
        }
        group->attackCooldowns[2] = group->attackDelays[2];
        Axe_SetAuxiliarySpriteRenderState(actor, group->redPalette, 0x13D);
        group->active = 1;
        Sound_PlaySong(0x16F);
    }
    if (actor->base.stepDone) {
        SceneActorChase_SetControl(actor, ActorPhase_UpdateSpriteCollapse, 0x23);
        Chase_SetState(actor, 0, 2, 7);
    } else {
        actor->base.counter2AC++;
    }
}

static inline u16 Mask16(u16 *p, u32 m)
{
    return *p & m;
}

void SceneActorControl_EndExtendedPhase(struct ChaseActor *actor)
{
    struct Axe *context = actor->base.context;
    if (SceneActorControl_TakeFlag(&actor->base.field2B1))
        context->lungeQueued = 1;
    if (actor->base.stepDone) {
        Axe_SetAuxiliarySpriteRenderState(actor, context->blackPalette, 0x13C);
        actor->base.motionFlags = Mask16(&actor->base.motionFlags, ~0x100);
        {
            u32 *flags = &context->flagsE4C;
            u32 mask = ~0x1000;
            *flags &= mask;
        }
        Chase_SetBehavior(actor, 3);
    } else {
        ++actor->base.counter2AC;
    }
}

void SceneActor_AdvanceOffsetApproach(void);

static inline void Restart_UpdateControl2APhase(struct ChaseActor *actor)
{
    actor->base.motionFlags = Mask16(&actor->base.motionFlags, ~0x100);
    SceneActorChase_SetControl(actor, ActorPhase_UpdateSpriteCollapse, 0x23);
    Chase_SetBehavior(actor, 4);
}

void SceneActorControl_UpdateControl2APhase(struct ChaseActor *actor)
{
    struct Axe *context = actor->base.context;
    if (SceneActorControl_TakeFlag(&actor->base.field2B1)) {
        SceneActorChase_SetControl(actor, SceneActor_AdvanceOffsetApproach, 0x2A);
        Or16(0x100, &actor->base.motionFlags);
        context->attackCooldowns[3] = context->attackDelays[3];
    }
    if (actor->base.stepDone) {
        Restart_UpdateControl2APhase(actor);
    } else {
        ++actor->base.counter2AC;
    }
}

struct AxePlayer {
    u8 pad[0x30];
    struct ContactPosition position;
};

struct AxeRoute {
    u8 pad[0xE5A];
    u16 speed;
    struct ContactPosition points[9];
    u8 index;
    u8 padEA5[0xF14 - 0xEA5];
    struct ContactPosition motion;
};

static inline u32 RandomFirst(u32 bits)
{
    u32 index, value;
    gRandomIndex = (gRandomIndex + 1) & 1023;
    index = gRandomIndex * 2;
    value = *(const u16 *)(index + RANDOM_TABLE_ADDR);
    return value & bits;
}

static inline u16 Random_FollowDeceleratingRoute(void)
{
    gRandomIndex = (gRandomIndex + 1) & 1023;
    return *(const u16 *)(gRandomIndex * 2 + (u32)gRandomTable);
}

static inline u16 RandomMotion(void)
{
    u32 offset;
    gRandomIndex = (gRandomIndex + 1) & 1023;
    offset = gRandomIndex * 2;
    return *(const u16 *)(offset + RANDOM_TABLE_ADDR);
}

static inline u32 Facing(u32 value, u8 *direction)
{
    return (value & 63) + *direction;
}

// confidence=low: follows nine stored route points and retargets around the player; action role tentative.

void Axe_FollowDeceleratingRoute(struct ChaseActor *actor)
{
    struct AxeRoute *route = actor->base.context;
    struct ContactPosition delta, target;
    if (SceneActorControl_TakeFlag(&actor->base.field2B1)) {
        u32 origin, savedOrigin;
        u32 angle;
        SceneActorChase_SetControl(actor, actor->base.onAlternateEntry, 1);
        route->index = 0;
        target = (*(struct AxePlayer **)gPlayers)->position;
        origin = (u32)gRandomTable + (PARTICLE_RANDOM_SAMPLE_ORIGIN - RANDOM_TABLE_ADDR);
        angle = RandomFirst(255);
        SceneRoute_ProjectRandomOffset(&delta, angle, 512, origin, &savedOrigin);
        target.x += delta.x;
        target.z += delta.z;
        Chase_Delta(&delta, &actor->base.body.position, &target);
        Polar(&route->motion,
              Facing(ParticleRandom_ReadRelativeSample(savedOrigin, (const u16 *)RANDOM_TABLE_ADDR),
                     &actor->base.tracking.heading) &
                  255,
              32);
        route->speed = 1;
    }
    Chase_Delta(&delta, &actor->base.body.position, &route->points[route->index]);
    Axe_EmitMotionTrail(actor);
    if (delta.x * delta.x + delta.z * delta.z > 1024) {
        actor->base.body.direction =
            Math_Atan2(route->points[route->index].x - actor->base.body.position.x,
                       route->points[route->index].z - actor->base.body.position.z);
        Polar(&delta, actor->base.body.direction, route->speed);
        actor->base.body.motion.x += delta.x;
        actor->base.body.motion.z += delta.z;
        route->motion = actor->base.body.motion;
    } else if (++route->index > 8) {
        Chase_Delta(&route->motion, &actor->base.body.position, &route->points[8]);
        route->motion.y = 0;
        actor->base.body.position.x = route->points[8].x;
        actor->base.body.position.z = route->points[8].z;
        route->index = 0;
        target = (*(struct AxePlayer **)gPlayers)->position;
        Polar(&delta, Facing(Random_FollowDeceleratingRoute(), &actor->base.tracking.heading) & 255,
              512);
        target.x += delta.x;
        target.z += delta.z;
        Chase_Delta(&delta, &actor->base.body.position, &target);
        route->speed >>= 1;
        return;
    }
    if (Mod(actor->base.counter2AC, 16) == 0 && route->speed <= 11)
        ++route->speed;
    ++actor->base.counter2AC;
}

static inline u16 Random_FollowAcceleratingRoute(void)
{
    gRandomIndex = (gRandomIndex + 1) & 1023;
    return *(const u16 *)(gRandomIndex * 2 + (u32)gRandomTable);
}

// confidence=low: follows nine stored route points and retargets around the player; action role tentative.

void Axe_FollowAcceleratingRoute(struct ChaseActor *actor)
{
    struct AxeRoute *route = actor->base.context;
    struct ContactPosition delta, target;
    if (SceneActorControl_TakeFlag(&actor->base.field2B1)) {
        u32 origin, savedOrigin;
        u32 angle;
        SceneActorChase_SetControl(actor, actor->base.onAlternateEntry, 1);
        route->index = 0;
        target = (*(struct AxePlayer **)gPlayers)->position;
        origin = (u32)gRandomTable + (PARTICLE_RANDOM_SAMPLE_ORIGIN - RANDOM_TABLE_ADDR);
        angle = RandomFirst(255);
        SceneRoute_ProjectRandomOffset(&delta, angle, 512, origin, &savedOrigin);
        target.x += delta.x;
        target.z += delta.z;
        Chase_Delta(&delta, &actor->base.body.position, &target);
        Polar(&route->motion,
              Facing(ParticleRandom_ReadRelativeSample(savedOrigin, (const u16 *)RANDOM_TABLE_ADDR),
                     &actor->base.tracking.heading) &
                  255,
              32);
        route->speed = 1;
    }
    Chase_Delta(&delta, &actor->base.body.position, &route->points[route->index]);
    Axe_EmitMotionTrail(actor);
    if (delta.x * delta.x + delta.z * delta.z > 1024) {
        actor->base.body.direction =
            Math_Atan2(route->points[route->index].x - actor->base.body.position.x,
                       route->points[route->index].z - actor->base.body.position.z);
        Polar(&delta, actor->base.body.direction, route->speed);
        actor->base.body.motion.x += delta.x;
        actor->base.body.motion.z += delta.z;
        route->motion = actor->base.body.motion;
    } else if (++route->index > 8) {
        Chase_Delta(&route->motion, &actor->base.body.position, &route->points[8]);
        route->motion.y = 0;
        actor->base.body.position.x = route->points[8].x;
        actor->base.body.position.z = route->points[8].z;
        route->index = 0;
        target = (*(struct AxePlayer **)gPlayers)->position;
        Polar(&delta, Facing(Random_FollowAcceleratingRoute(), &actor->base.tracking.heading) & 255,
              256);
        target.x += delta.x;
        target.z += delta.z;
        Chase_Delta(&delta, &actor->base.body.position, &target);
        return;
    }
    if (Mod(actor->base.counter2AC, 16) == 0 && route->speed <= 17)
        ++route->speed;
    ++actor->base.counter2AC;
}

extern void *gUnk_08606468[];

void Axe_InitActionCallbacks(struct ChaseActor *actor)
{
    actor->base.behaviorCallbacks = gUnk_08606468;
}

void Axe_UpdateWaitingAnimation(struct RoomActor *actor, s32 frame);

static inline u16 SetFlag(struct ChaseActor *actor, u32 mask)
{
    u16 *flags = &actor->base.motionFlags;
    return *flags = mask | *flags;
}

void Axe_UpdatePlayerDestinationPhase(struct ChaseActor *actor)
{
    struct SceneActorRegistryControlGroup *group = &actor->group.common;
    if (SceneActorControl_TakeFlag(&actor->base.field2B1)) {
        SceneActorChase_SetControl(actor, Axe_UpdateWaitingAnimation, 10);
        SetFlag(actor, 0x100);
        group->gate.counter = group->gate.duration;
    }
    if (actor->base.stepDone)
        Axe_BeginMoveToPosition(actor, ((u8 **)gPlayers)[0] + 0x30);
    else
        actor->base.counter2AC++;
}

// confidence=low: after the wait, selects a route point and starts the stored movement spline.

static inline u32 CellIndex_UpdateRouteDestinationPhase(s32 x, s32 z)
{
    if (x < 0 || z < 0 || (u32)x >= gTerrainWidth || (u32)z >= gTerrainHeight)
        return 0;
    return gTerrainMap->rows[z] + x;
}

static inline void SetState_UpdateRouteDestinationPhase(struct ChaseActor *actor, u8 state,
                                                        u8 phase, u8 behavior, u32 counter)
{
    actor->base.state2A8 = state;
    actor->base.phase2A9 = phase;
    actor->base.behavior2AA = behavior;
    actor->base.counter2AC = counter;
    actor->base.field2B1 = 1;
}

void Axe_UpdateRouteDestinationPhase(struct ChaseActor *actor)
{
    struct Axe *context = actor->base.context;
    struct AxeVec targetStorage;
    struct AxeVec *target;
    struct AxeVec *copy;
    struct SceneActorChaseRoutePoint *point;
    struct SceneActorRegistryRoute *link;
    s16 deltaX;
    s16 deltaZ;
    if (actor->base.field2B1)
        actor->base.field2B1 = 0;
    if ((u8)Callback_InvokeActor(actor, actor->base.contactQuery.opaque))
        return;
    if (((s32)actor->base.counter2AC) > 60) {
        Axe_SelectRandomRoutePoint(actor);
        target = &targetStorage;
        link = &actor->group.homeRoute;
        point = link->point;
        target->x = point->x;
        target->z = point->z;
        {
            s32 cellX = target->x >> 8, cellZ = target->z >> 8;
            copy = target;
            target->y = CellHeight(target, CellIndex_UpdateRouteDestinationPhase(cellX, cellZ));
        }
        Axe_SetMovementEndpoints(actor, copy);
        Axe_StepCurveMotion(actor);
        SetState_UpdateRouteDestinationPhase(actor, 0, 0, 1, 0);
        {
            struct AxeVec *position = (struct AxeVec *)(&actor->base.body.position);
            target = &targetStorage;
            target->x -= position->x;
            copy->y -= position->y;
            copy->z -= position->z;
        }
        deltaZ = copy->z;
        deltaX = target->x;
        actor->base.nextDirection = Math_Atan2(deltaX, deltaZ);
        ZeroFill8(context->clearF14);
    } else
        actor->base.counter2AC++;
}

extern void *gUnk_08606494[];

void Axe_InitEventCallbacks(struct ChaseActor *p)
{
    p->base.alternateBehaviorCallbacks = gUnk_08606494;
}

s32 SceneActorControl_AdvanceReactionCount(struct ChaseActor *actor)
{
    struct Axe *context = actor->base.context;
    u16 *flags;
    u32 mask;
    if (actor->base.reactionSoundVariant == 0)
        Sound_PlaySong(0x211);
    else
        Sound_PlaySong(0x13E);
    flags = &actor->base.motionFlags;
    mask = ~0x20;
    *flags = mask & *flags;
    if (*flags & 0x8000)
        context->reactionHoldTimer = 8;
    if (context->reactionCount <= 2)
        ++context->reactionCount;
    if (!SceneActorTurn_Flags(flags, 0x8100)) {
        u32 blocked = WordFlags_Has(4, &context->flagsE4C);
        if (!blocked && context->reactionCount > 2) {
            Chase_SetState(actor, 0, 2, 6);
            context->reactionCount = 0;
            return 1;
        }
    }
    return 1;
}

s32 Axe_PrepareUpdate(void *p);

s32 Axe_UpdateEffects(void *s);

s32 Callback_InvokeActor(void *, void *);

void SceneActorControl_DispatchControl(void *);

s32 Axe_OnUpdate(struct ChaseActor *p)
{
    u8 idx;
    u32 *tbl;

    Axe_PrepareUpdate(p);
    idx = p->base.state2A8;
    tbl = (u32 *)p->base.stateCallbacks;
    Callback_InvokeActor(p, tbl[idx]);
    SceneActorControl_DispatchControl(p);
    Axe_UpdateEffects(p);
    return 1;
}

void SceneActor_UpdateBehaviorCallbacks(struct ChaseActor *);

s32 Axe_OnRender(struct ChaseActor *actor)
{
    actor->base.tracking.target = &((struct SceneActorChasePlayer *)gPlayers[0])->target;
    SceneActor_UpdateBehaviorCallbacks(actor);
    return 0;
}

void Axe_RemoveSecondaryColliders(struct ChaseActor *actor);

void Axe_RemoveAuxiliarySprites(struct ChaseActor *actor);

void SceneActorEffects_ReleaseResources(void *);

s32 Axe_ReleaseResources(void *p)
{
    Axe_RemoveSecondaryColliders(p);
    Axe_RemoveAuxiliarySprites(p);
    Axe_ReleaseAuxiliarySprite(p);
    SceneActorEffects_ReleaseResources(p);
    return 0;
}

void Axe_InitProfile(struct ChaseActor *actor);

void Axe_InitAttackCooldowns(struct ChaseActor *actor);

void ActorGroup_ReadScriptOptions(struct ChaseActor *actor);

void Axe_InitEffectPalettes(struct ChaseActor *actor);

void Axe_InitColliders(struct ChaseActor *actor);

s32 Axe_OnUpdate(struct ChaseActor *p);

s32 Axe_GetActorCategory(void);

void Axe_UpdateTurnAnimation(struct RoomActor *actor, u32 events);

void Axe_OnAnimationReset(struct ChaseActor *actor);

void Axe_BeginProfileEntry(struct ChaseActor *actor);

void Axe_ConvergeRecoveryMotion(struct ChaseActor *actor);

void Axe_UpdateDefeat(struct ChaseActor *actor, s32 frame);

s32 SceneActorControl_CanAdvanceHover(struct ChaseActor *actor);

void SceneActorTarget_ResolvePendingObject(void);

s32 SceneActorControl_AdvanceReactionCount(struct ChaseActor *actor);

void SceneActorControl_InitializeHoverState(struct ChaseActor *actor);

void Axe_OnReaction(void);

void Axe_ReleaseAndIdle(struct ChaseActor *actor);

void Axe_UpdateDamageSpin(struct ChaseActor *actor, s32 frame);

void SceneActorBehavior_InitializeShadow(void *);

void Axe_InitMotionTable(struct ChaseActor *actor);

void Axe_InitActionCallbacks(struct ChaseActor *actor);

void Axe_InitEventCallbacks(struct ChaseActor *p);

void SpriteGroup_InitAnimatedSprites(struct ChaseActor *owner);

void Axe_InitRoutePointHistory(struct ChaseActor *actor);

void ActorCollision_InitThreeAuxiliaryAttacks(struct ChaseActor *actor);

s32 Axe_OnRender(struct ChaseActor *actor);

s32 Axe_ReleaseResources(void *p);

void Memory_Clear(void *dst, s32 bytes); /* zero fill (bytes, then CpuSet words) */

void SceneActorReaction_OnEnemyDefault(void);

u8 SceneActorTarget_UpdateTrackedContact(void *);

void SceneActorReaction_SinkAndRemove(void);

void SceneActorReaction_ResolvePlacementDamage(void);

void SceneActorPlacement_ResetStateThree(void);

void SceneActorReaction_BeginTargetRecovery(void);

void SceneActorScript_OnCommand(void *actor, const u16 *command);

void SceneActorMovement_ResolveRoutePosition(void *);

void SceneActorPlacement_HideActor(void *, s32);

void SceneActorAnimation_SelectDirectionalEntry(struct RoomActor *, s32);

void SceneActorBehavior_InstallCallbackTables(void *);

void SceneActorTerrain_ReadScriptParameters(void *);

void *Heap_Alloc(s32 size); /* first-fit allocation from gHeapHead; NULL on failure */

void SceneActorRegistry_SetObjectContext(void *, void *, s32, s32);

extern const u8 gUnk_086108B0[];

s32 Anim_Step(void *, void *);

u8 SceneActorRegistry_LinkActor(void *);

struct AxeActorInit {
    struct ChaseActor base;
    u8 unknown624[0x824];
    s32 halfHealthE48;
    u32 savedFlagsE4C;
    u8 unknownE50[8];
    u16 groundHeightE58;
    u8 unknownE5A[0x5C];
    u16 fieldEB6;
    u8 unknownEB8[0x1F];
    u8 variantED7;
    u8 unknownED8[0x44];
    struct SceneVec playerPositionF1C;
};

static inline void AxeInit_InstallPrimary(struct ChaseActor *arg0, void *cb0, s32 (*cb1)(void),
                                          void *cb2, void *cb3, void *cb4, void *hidden, void *cb5,
                                          void *cb6, void *defaultReaction, s32 (*cb270)(void *),
                                          void *targetQuery, void *damage, void *contact)
{
    MonsterInit_SetBehaviorFlags(arg0, 0x2000);
    arg0->base.onUpdate = cb0;
    MonsterInit_WriteValueQuery(&arg0->base.animationQuery.value, cb1);
    arg0->base.onEntry = cb2;
    arg0->base.onAlternateEntry = cb3;
    arg0->base.onStanding = cb4;
    arg0->base.onReaction = cb5;
    arg0->base.onDefeat = cb6;
    arg0->base.onHidden = hidden;
    arg0->base.defaultReaction = defaultReaction;
    MonsterInit_WriteResultCallback(&arg0->base.contactQuery.result, cb270);
    MonsterInit_WriteDataPointer(&arg0->base.commandTable.opaque, gUnk_086108B0);
    arg0->base.routeReset = 0;
    arg0->base.targetQuery = targetQuery;
    arg0->base.sinkCallback = SceneActorReaction_SinkAndRemove;
    arg0->base.onDamage = damage;
    arg0->base.onContact = contact;
    arg0->base.field240 = 0;
    MonsterInit_ClearMotion(arg0);
}

static inline void AxeInit_InstallCompletionCallbacks(struct ChaseActor *arg0)
{
    arg0->base.onMessage = SceneActorScript_OnCommand;
    arg0->base.update = (SceneActorResultCallback)Axe_OnRender;
    arg0->base.destroy = (SceneActorResultCallback)Axe_ReleaseResources;
}

static inline void AxeInit_InstallRoutes(struct ChaseActor *actor)
{
    actor->base.routeReset = SceneActorControl_InitializeHoverState;
    actor->base.reactionGate = Axe_OnReaction;
}

static inline u8 AxeInit_BoundedVariant(s32 variant)
{
    if (variant > 1)
        return 1;
    return variant;
}

static inline void AxeInit_SetVariant(struct ChaseActor *actor, u8 variant)
{
    ((struct AxeActorInit *)actor)->variantED7 = variant;
}

static inline void AxeInit_InitializePaletteRange(struct ChaseActor *actor, u32 alternate,
                                                  u32 current)
{
    actor->base.alternatePalette = MonsterInit_GetPaletteBase(actor) + alternate;
    actor->base.paletteBank = MonsterInit_GetPaletteBase(actor) + current;
}

s32 Axe_Init(void *object)
{
    struct AxeActorInit *arg0 = object;
    struct ChaseActor *actor = &arg0->base;
    struct SceneActorChaseGroup *group = &arg0->base.group;
    // concession-ok(const-local): The CpuSet position clears use the same fixed source word.
    u32 zero;
    struct ContactPosition tmp;
    u32 flag;
    s32 variant;

    arg0->savedFlagsE4C = 0;
    SceneActorRegistry_SetObjectContext(actor, actor, 0, 1);
    ((struct AxeActorInit *)actor)->variantED7 = Script_GetKeywordValue('t', 0);
    variant = AxeInit_BoundedVariant(((struct AxeActorInit *)actor)->variantED7);
    AxeInit_SetVariant(&arg0->base, variant);
    if (variant == 0)
        MonsterInit_SetCategory(actor, MONSTER_CATEGORY_AXE);
    else
        MonsterInit_SetCategory(actor, MONSTER_CATEGORY_AXE_VARIANT);
    Axe_InitProfile(actor);
    if (MonsterInit_GetDrawableKind(actor) == 0) {
        u8 *heap;
        actor->base.drawable = Heap_Alloc(0x5C);
        Memory_Clear(actor->base.drawable, 0x5C);
        heap = MonsterInit_Drawable(actor);
        MonsterInit_InitializeSprite(heap, 0x5291, 0x3F72);
        if (MonsterInit_GetCategory(actor) == 0x11 || MonsterInit_GetCategory(actor) == 0x17)
            heap[7] = 1;
        actor->base.body.direction = 0;
    } else {
        u8 *heap;
        actor->base.drawable = Heap_Alloc(0x80);
        Memory_Clear(actor->base.drawable, 0x80);
        heap = MonsterInit_Drawable(actor);
        MonsterInit_ClearPosition(&zero, &tmp);
        MonsterInit_InitializeWidget(heap, 0xCB05, 0x5291, &tmp);
        actor->base.animationIndex = 0xFF;
    }
    MonsterInit_InitializePursuitBody(&arg0->base, &zero);
    arg0->halfHealthE48 = MonsterInit_GetHealth(actor) >> 1;
    Axe_InitAttackCooldowns(actor);
    if (Script_SeekToKeyword('p')) {
        u32 height;
        actor->base.body.position.x = MonsterInit_ReadHalfword(0);
        actor->base.body.position.y = MonsterInit_ReadHalfword(0);
        actor->base.body.position.z = MonsterInit_ReadHalfword(0);
        actor->base.terrainCell = MonsterInit_CellIndex(&actor->base.body.position);
        height = MonsterInit_TerrainHeight(MonsterInit_GetTerrainCell(actor),
                                           &actor->base.body.position);
        actor->base.groundHeight = height;
        arg0->groundHeightE58 = height;
    } else {
        arg0->groundHeightE58 = actor->base.groundHeight;
    }
    arg0->fieldEB6 = Script_GetKeywordValue('w', -1);
    ActorGroup_ReadScriptOptions(actor);
    arg0->playerPositionF1C = gPlayers[0]->position;
    MonsterInit_ReadPalette(actor, 0x2BD, 0x192);
    AxeInit_InitializePaletteRange(actor, 0, 2);
    Axe_InitEffectPalettes(actor);
    MonsterInit_SetAlternatePaletteOffset(actor, 0x1EE);
    Axe_InitColliders(actor);
    AxeInit_InstallPrimary(
        actor, Axe_OnUpdate, Axe_GetActorCategory, Axe_UpdateTurnAnimation, Axe_OnAnimationReset,
        Axe_BeginProfileEntry, 0, Axe_ConvergeRecoveryMotion, Axe_UpdateDefeat,
        SceneActorReaction_OnEnemyDefault, SceneActorControl_CanAdvanceHover,
        SceneActorTarget_ResolvePendingObject, SceneActorReaction_ResolvePlacementDamage,
        SceneActorControl_AdvanceReactionCount);
    MonsterInit_InitializeGroup(group, (u8 *)arg0 + 0xED8, (u8 *)arg0 + 0xEFC, 0, 0, 0, 0, 1);
    AxeInit_InstallRoutes(actor);
    group->responseCallback = 0;
    group->routeRestoreCallback = 0;
    group->releaseControl = Axe_ReleaseAndIdle;
    group->heldControl = Axe_UpdateDamageSpin;
    MonsterInit_SetMotionFlags(actor, 0x60);
    MonsterInit_SetBehaviorFlags(actor, 0x200);
    MonsterInit_SetStatusFlags(actor, 0x02100408);
    SceneActorBehavior_InitializeShadow(actor);
    SceneActorTerrain_ReadScriptParameters(actor);
    flag = 0;
    if (MonsterInit_TestMotionFlags(actor, 4)) {
        MonsterInit_SetStatusFlags(actor, 0x2000);
        flag = 1;
    }
    if (flag != 1) {
        MonsterInit_SetState(actor, 0, 0, 0);
        MonsterInit_SetControl(actor, MonsterInit_GetStandingControl(actor), 0);
    }
    MonsterInit_InitializeRegion(actor);
    if (MonsterInit_TestMotionFlags(actor, 0x40))
        MonsterInit_MaskColliderFlags(&actor->base.colliderA, ~0x100);
    if (Script_GetKeywordValue('S', 0) != 0 || !MonsterInit_IsInCurrentRegion(actor))
        SceneActorPlacement_HideActor(actor, 0);
    SceneActorAnimation_SelectDirectionalEntry(actor, 0);
    if (MonsterInit_GetDrawableKind(actor) == 0)
        DisplaySequence_Step(actor->base.drawable);
    else
        Anim_Step(actor->base.drawable + 0x20, actor->base.drawable);
    MonsterInit_ClearReceiver(actor);
    SceneActorBehavior_InstallCallbackTables(actor);
    Axe_InitMotionTable(actor);
    Axe_InitActionCallbacks(actor);
    Axe_InitEventCallbacks(actor);
    MonsterInit_Drawable(actor)[7] = 1;
    actor->base.field2E8 = 0;
    SpriteGroup_InitAnimatedSprites(actor);
    Axe_InitRoutePointHistory(actor);
    ActorCollision_InitThreeAuxiliaryAttacks(actor);
    MonsterInit_SaveControl(MonsterInit_SavedControlOf(actor), actor);
    actor->base.saved.privateFlags = arg0->savedFlagsE4C;
    AxeInit_InstallCompletionCallbacks(actor);
    if (!SceneActorRegistry_LinkActor(actor))
        return -1;
    return 0;
}

s32 Axe_Init(void *object);

void Heap_Free(void *);

void Axe_Create(void)
{
    u8 *p = Heap_Alloc(0xf24);
    if (p != 0) {
        Memory_Clear(p, 0xf24);
        if (Axe_Init(p) < 0) {
            Axe_ReleaseResources(p);
            Heap_Free(p);
        }
    }
}
