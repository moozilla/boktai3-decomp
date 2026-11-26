#include "engine/shared/scene_actor_displacement.h"
#include "engine/shared/scene_actor_chase.h"
#include "monsters/slime/tentacle_spread.h"
#include "monsters/slime/slime.h"
#include "engine/shared/scene_actor_turn.h"
#include "engine/shared/scene_actor_visibility.h"
#include "engine/shared/engine.h"
#include "engine/shared/ram.h"
#include "engine/shared/scene_actor_registry.h"
#include "actors/scene_actor_behavior/scene_actor_behavior.h"
#include "engine/shared/scene_actor_control_phase.h"
#include "engine/script_utility_references/script_utility.h"
#include "engine/shared/scene_actor_reset.h"
#include "engine/shared/scene_actor_control_motion.h"
#include "actors/scene_actor_animation_and_scripted_actions/scene_actor_animation.h"
#include "engine/shared/display_sequence_inline.h"
#include "engine/shared/scene_actor_chase_reaction.h"
#include "engine/shared/scene_actor_contact_response.h"
#include "engine/shared/scene_actor_terrain.h"
#include "engine/shared/engine_globals.h"
#include "engine/shared/scene_actor_motion.h"
#include "engine/shared/player.h"
#include "engine/shared/scene_actor_state.h"
#include "engine/scene_body/scene_body_api.h"
#include "engine/shared/render.h"
#include "engine/shared/scene_actor_init_group.h"
#include "engine/shared/scene_actor_init_graphics.h"
#include "engine/shared/scene_actor_terrain_reaction.h"
#include "monsters/ghoul/ghoul_disc.h"

void Slime_ApplyBodyHit(struct ContactCollider *target, struct ContactCollider *info,
                        struct ChaseActor *actor)
{
    if (!SceneActor_ActionsBlocked(actor)) {
        switch (info->contactFlags) {
            case 1:
                if (!SceneActor_ActionsBlocked((void *)actor) && (info->contactFlags & 0xF)) {
                    actor->base.counter304 = info->collisionId;
                    {
                        u32 *field = &actor->base.statusFlags;
                        u32 mask = ~0x1000000;
                        *field &= mask;
                    }
                }
                if (actor->base.paletteOffset == 3) {
                    target->reactionFlags = Slime_GetRandomVariantFlags((u8 *)actor);
                }
        }
    }
}

// confidence=low: terrain-edge response is registered as the Slime body collider callback.

void SceneActorBehavior_ApplyContactReaction(struct SceneActorBehaviorContactView *,
                                             struct SceneActorBehaviorContactView *,
                                             struct RoomActor *);

static inline u8 Slime_ContactUnavailable(struct ChaseActor *actor)
{
    if (actor) {
        u32 mask = 0x40;
        if (actor->base.motionFlags & mask)
            return 1;
        mask = 0x20000;
        if (actor->base.statusFlags & mask)
            return 1;
        if (actor->base.health <= 0)
            return 1;
    }
    return 0;
}

static inline void Slime_AddPositions(struct ContactPosition *out, struct ContactPosition *a,
                                      struct ContactPosition *b)
{
    out->x = b->x + a->x;
    out->y = b->y + a->y;
    out->z = b->z + a->z;
}


static inline s32 Slime_QuarterDirectionForContact(s32 angle)
{
    return angle >= 0 ? angle >> 6 : -(-angle >> 6);
}

static inline u32 Slime_ContactFlags(struct ContactCollider *contact, u32 mask)
{
    return contact->flags & mask;
}

void Slime_ResolveBodyContact(struct ContactCollider *contact, void *other,
                              struct ChaseActor *actor)
{
    struct ContactPosition next, displacement, center, fromContact, adjusted;
    s32 angle;
    u32 contactFlags;
    struct ChaseActor *checkActor = actor;
    if (Slime_ContactUnavailable(checkActor))
        return;
    contactFlags = Slime_ContactFlags(contact, 0x800);

    if (contactFlags) {
        Slime_AddPositions(&next, &actor->base.body.position, &actor->base.body.motion);
        SceneActorTerrain_Subtract(&displacement, (void *)&contact->x, &next);
        SceneActorTerrain_GetCenter(actor->base.terrainCell, &center);
        SceneActorTerrain_Subtract(&fromContact, &center, (void *)&contact->x);
        angle = Slime_QuarterDirectionForContact(Math_Atan2(fromContact.x, fromContact.z) + 32);
        switch (angle) {
            case 0:
                displacement.x += 0xC0;
                displacement.z = angle;
                break;
            case 1:
                displacement.x = 0;
                displacement.z += 0xC0;
                break;
            case 2:
                displacement.x -= 0xC0;
                displacement.z = 0;
                break;
            case 3:
                displacement.x = 0;
                displacement.z -= 0xC0;
                break;
        }
        Slime_AddPositions(&adjusted, &actor->base.body.position, &displacement);
        {
            s32 cell = SceneActorChase_CellAt(&adjusted);
            u32 ground = actor->base.groundHeight;
            u8 *data = SceneActorTerrain_GetCell(cell);
            if (ground != (*data & 15) << 8)
                SceneActorMotion_ArmPrimaryCollider(actor);
            else {
                actor->base.body.position.x += displacement.x;
                actor->base.body.position.z += displacement.z;
                actor->base.body.motion.x = 0;
                actor->base.body.motion.z = 0;
            }
        }
    } else
        SceneActorBehavior_ApplyContactReaction((void *)contact, other, (void *)checkActor);
}

void Slime_ResetAnimation(struct ChaseActor *actor)
{
    if (actor->base.field2B2 != 0)
        actor->base.field2B2 = 0;
    SceneActorAnimation_SelectProfileEntry((void *)actor, 1);
}

void Slime_StartWalkChoice(struct ChaseActor *actor)
{
    if (Slime_TakePrepared(actor)) {
        SceneActorAnimation_SelectProfileEntry((void *)actor, 1);
        actor->base.field2C2 = 0x40;
        Anim_SetDurationScale(
            &((struct SceneActorAnimationSequenceDrawable *)actor->base.drawable)->sequence, 0x40);
    }
    if (actor->base.restart) {
        s32 r;
        gRandomIndex = (gRandomIndex + 1) & 1023;
        r = *(u16 *)(gRandomIndex * 2 + (u32)gRandomTable);
        r &= 31;
        if (r <= 23)
            SceneActorChase_SetControl(actor, Slime_RollMoveFlag, 0);
        else
            SceneActorChase_SetControl(actor, Slime_StartWalkChoice, 0x29);
    }
}

void Slime_RollMoveFlag(struct ChaseActor *actor)
{
    if (Slime_TakePrepared(actor)) {
        u16 *scale = &actor->base.field2C2;
        *scale = 0x40;
        Anim_SetDurationScale(
            &((struct SceneActorAnimationSequenceDrawable *)actor->base.drawable)->sequence, 0x40);
        SceneActorAnimation_SelectProfileEntry((void *)actor, 0x10);
    }
    {
        u32 *cur = &gRandomIndex;
        u32 index;
        s32 roll;
        index = (*cur + 1) & 0x3ff;
        *cur = index;
        roll = *(u8 *)(index * 2 + RANDOM_TABLE_ADDR);
        if (roll <= 7) {
            u32 mask = 0x10;
            u16 *flags = &actor->base.reactionFlags;
            mask |= *flags;
            *flags = mask;
        } else if (roll <= 13) {
            u32 mask = 0x20;
            u16 *flags = &actor->base.reactionFlags;
            mask |= *flags;
            *flags = mask;
        }
    }
}

void Slime_StartRandomWait(struct ChaseActor *actor)
{
    if (Slime_TakePrepared(actor)) {
        SceneActorAnimation_SelectProfileEntry((void *)actor, 0x11);
        actor->base.field2C2 = 0x40;
        Anim_SetDurationScale(
            &((struct SceneActorAnimationSequenceDrawable *)actor->base.drawable)->sequence, 0x40);
    }
    if (actor->base.restart)
        SceneActorChase_SetControl(actor, Slime_UpdateRandomWait, 0x22);
}

void Slime_UpdateReturnToIdle(struct ChaseActor *actor)
{
    if (Slime_TakePrepared(actor)) {
        SceneActorAnimation_SelectProfileEntry((void *)actor, 0x12);
        actor->base.field2C2 = 0x40;
        Anim_SetDurationScale(
            &((struct SceneActorAnimationSequenceDrawable *)actor->base.drawable)->sequence, 0x40);
    }
    if (actor->base.restart) {
        u8 *q = &actor->base.state2A8;
        u32 z;
        z = 0;
        *q = z;
        actor->base.phase2A9 = z;
        actor->base.behavior2AA = z;
        actor->base.counter2AC = z;
        actor->base.field2B1 = 1;
        SceneActorChase_SetControl(actor, SceneActorControl_OnIdle, 0x10);
    }
}

// confidence=low: plays profile animation 15 with a random duration, then enters the next Slime control.

static inline void Slime_SelectWaitAnimation(struct RoomActor *actor, s32 index, u32 variant,
                                             u32 inputFlags)
{
    struct SceneActorProfileEntry *entry =
        (void *)SceneActorProfiles_GetEntryAddress(actor->profileId, actor->profileMode, index);
    u32 selection = entry->value;
    u32 flags = inputFlags;
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
        Anim_SetSequence(&drawable->sequence, (void *)drawable->resource, selection, variant,
                         flags);
        restart = &actor->restart;
    } else {
        struct SceneInputWidget *tile =
            &((struct SceneActorAnimationTileDrawable *)actor->drawable)->tile;
        u8 *selectionAddress;
        u32 previous;
        u8 *savedSelection;
        selection += variant;
        selectionAddress = &actor->animationIndex;
        previous = *selectionAddress;
        savedSelection = selectionAddress;
        restart = &actor->restart;
        if (!force && previous == selection && !*restart) {
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
                              actor->drawable, (u16)selection, 2, mode);
            {
                u8 playbackFlags = flags;
                if (playbackFlags & 1)
                    tile->flags |= 4;
                else
                    tile->flags &= ~4;
            }
        }
    }
    *restart = 0;
}

void Slime_UpdateRandomWait(struct ChaseActor *actor)
{
    if (Slime_TakePrepared(actor)) {
        u32 index;
        Slime_SelectWaitAnimation((void *)actor, 15, 0, 0);
        index = (gRandomIndex + 1) & 1023;
        gRandomIndex = index;
        {
            u32 random = *(u16 *)(index * 2 + (u32)gRandomTable);
            actor->base.field2C2 = random & 63;
        }
        if (!actor->base.field2C2)
            actor->base.field2C2 = 64;
        Anim_SetDurationScale(
            &((struct SceneActorAnimationSequenceDrawable *)actor->base.drawable)->sequence,
            actor->base.field2C2);
    }
    if (actor->base.restart)
        SceneActorChase_SetControl(actor, Slime_PlayWaitAnimation, 0x23);
}

void Slime_PlayWaitAnimation(struct ChaseActor *actor)
{
    if (Slime_TakePrepared(actor))
        SceneActorTurn_SelectProfileAnimation((void *)actor, 15, 1, 0);
    if (actor->base.restart)
        SceneActorChase_SetControl(actor, Slime_TickWaitTimer, 0x24);
}

void Slime_TickWaitTimer(struct ChaseActor *actor)
{
    struct Slime *q = actor->base.context;
    if (Slime_TakePrepared(actor)) {
        u32 index = (gRandomIndex + 1) & 1023;
        u32 random;
        gRandomIndex = index;
        random = *(u16 *)(index * 2 + (u32)gRandomTable);
        q->waitTimer = (random & 63) + 0x40;
    }
    if (q->waitTimer)
        q->waitTimer--;
    else if (actor->base.restart)
        SceneActorChase_SetControl(actor, Slime_UpdateRandomWait, 0x22);
}

void Anim_SetDurationScale(void *, u32);


void Slime_UpdateWalkTransition(struct ChaseActor *p)
{
    if (Slime_TakePrepared(p)) {
        SceneActorAnimation_SelectProfileEntry((void *)p, 0xe);
        Anim_SetDurationScale(
            &((struct SceneActorAnimationSequenceDrawable *)p->base.drawable)->sequence, 0x40);
    }
    if (p->base.restart != 0) {
        u32 index = (gRandomIndex + 1) & 1023;
        s32 r;
        gRandomIndex = index;
        r = *(u16 *)(index * 2 + (u32)gRandomTable);
        r &= 0x1f;
        if (r <= 0x17)
            SceneActorChase_SetControl(p, Slime_RollMoveFlag, 0);
        else
            SceneActorChase_SetControl(p, Slime_StartWalkChoice, 0x29);
    }
}

void Slime_OnUnusedControlEvent(void)
{
}

// confidence=low: motion converges to saved displacement before choosing idle or the next Slime control.


static inline s32 Slime_ClampMotion(s32 v)
{
    if (v > 200)
        return 200;
    if (v < -200)
        return -200;
    return v;
}

static inline u8 Slime_MotionAtRest(s32 x, s32 z)
{
    if (!x && !z)
        return 1;
    return 0;
}

static inline u8 Slime_HasContextFlag(struct Slime *p, u32 mask)
{
    if (p->flags & mask)
        return 1;
    return 0;
}

static inline u32 Slime_IsReactionComplete(struct ChaseActor *actor)
{
    u32 complete = actor->base.restart;
    return complete;
}

static inline u32 *Slime_EnableReactionMotion(struct ChaseActor *actor, u32 bits)
{
    u32 *flags = &actor->base.behaviorFlags;
    *flags |= bits;
    return flags;
}

void Slime_ConvergeReactionMotion(struct ChaseActor *actor)
{
    struct Slime *context = actor->base.context;
    struct DisplaySequenceState *sequence =
        &((struct SceneActorAnimationSequenceDrawable *)actor->base.drawable)->sequence;
    s32 x;
    s32 z;
    s16 *priorX;
    s16 *priorZ;
    u32 *actorFlags;
    u32 *t;
    if (Slime_TakePrepared(actor)) {
        actor->base.secondaryMotion.x = SceneActorDisplacement_Quarter(actor->base.secondaryMotion.x);
        actor->base.secondaryMotion.z = SceneActorDisplacement_Quarter(actor->base.secondaryMotion.z);
        SceneActorAnimation_SelectProfileEntry((void *)actor, 3);
        {
            u16 *flags = &actor->base.motionFlags;
            u32 mask = ~1;
            *flags = mask & *flags;
        }
        actor->base.restart = 0;
    }
    t = Slime_EnableReactionMotion(actor, 0x2000);
    MonsterInit_SetStatusFlags(actor, 0x10000);
    {
        s32 current = actor->base.reactionStart.x;
        s16 *field = &actor->base.reactionEnd.x;
        s32 difference = current - *field;
        actorFlags = t;
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
    x = Slime_ClampMotion(x);
    z = Slime_ClampMotion(z);
    actor->base.body.motion.x += x;
    actor->base.body.motion.z += z;
    *priorX += x;
    *priorZ += z;
    {
        u32 rest = Slime_MotionAtRest(x, z);
        if (!rest)
            sequence->elapsed = rest;
        else if (Slime_IsReactionComplete(actor)) {
            actor->base.stepDone = 1;
            SceneActorChase_ClearMotionFlagComponent(actor, 0x20);
            {
                u32 mask = ~0x80;
                *actorFlags = mask & *actorFlags;
            }
            if (actor->base.health < context->halfHealth || Slime_HasContextFlag(context, 8)) {
                SceneActorChase_SetControl(actor, SceneActorControl_OnIdle, 0x10);
                SceneActorChase_SetBehavior(actor, 2);
                {
                    u16 *flags = &actor->base.reactionFlags;
                    u32 mask = ~0x980;
                    *flags = mask & *flags;
                }
            } else {
                SceneActorChase_SetControl(actor, Slime_RollMoveFlag, 0);
                SceneActorChase_SetState(actor, 0, 0, 0);
            }
        }
    }
}

void Slime_UpdateShake(struct ChaseActor *actor, s32 elapsed)
{
    struct Slime *q = actor->base.context;
    struct DisplaySequenceState *st =
        &((struct SceneActorAnimationSequenceDrawable *)actor->base.drawable)->sequence;
    if (Slime_TakePrepared(actor)) {
        q->enrageCount = 0;
        actor->base.field2C2 = 0x40;
        Anim_SetDurationScale(
            &((struct SceneActorAnimationSequenceDrawable *)actor->base.drawable)->sequence, 0x40);
        SceneActorAnimation_SelectProfileEntry(actor, 1);
        SceneActorReaction_EnableMotionFlags(0x100, &actor->base.motionFlags);
    }
    st->index = 0;
    st->elapsed = 5;
    if (Mod(elapsed, 4) == 0) {
        u32 mask = 0x20;
        u16 *flags = &actor->base.reactionFlags;
        mask |= *flags;
        *flags = mask;
    }
    if (elapsed > 30)
        SceneActorChase_SetControl(actor, Slime_UpdateLaunch, 0x2B);
}

extern const s16 gUnk_086149C4[];

static inline s32 Shift12(s32 x)
{
    if (x >= 0)
        return x >> 12;
    return -((-x) >> 12);
}

void Slime_UpdateLaunch(struct ChaseActor *actor)
{
    struct SceneActorAnimationSequenceDrawable *d = actor->base.drawable;
    u32 mask;
    struct DisplaySequenceState *anim = &d->sequence;
    if (Slime_TakePrepared(actor)) {
        struct ContactPosition *v;
        u32 heading;
        s32 speed;
        u8 *hp;
        actor->base.field2C2 = 0x20;
        Anim_SetDurationScale(
            &((struct SceneActorAnimationSequenceDrawable *)actor->base.drawable)->sequence, 0x40);
        hp = &actor->base.tracking.heading;
        v = &actor->base.secondaryMotion;
        heading = *hp;
        speed = 0x12;
        v->x = Shift12(gUnk_086149C4[(heading + 0x40) & 0xFF] * speed);
        v->y = 0;
        v->z = Shift12(gUnk_086149C4[heading] * speed);
        actor->base.secondaryMotion.y = 0x28;
        /* The affine renderer uses the palette bytes as signed scales. */
        d->sprite.flags |= 2;
        d->sprite.palette = 0x40;
        d->sprite.paletteVariant = 0x40;
        Sound_PlaySong(0x3C1);
        actor->base.field2E8 = 0;
    }
    if (anim->index == 3)
        anim->elapsed = 0;
    if (actor->base.secondaryMotion.y > 0) {
        if ((s8)d->sprite.paletteVariant <= 0x7E)
            d->sprite.paletteVariant = (s8)d->sprite.paletteVariant + 1;
    } else {
        if ((s8)d->sprite.paletteVariant > 0x40)
            d->sprite.paletteVariant = (s8)d->sprite.paletteVariant - 2;
        if ((s8)d->sprite.paletteVariant <= 0x3F)
            d->sprite.paletteVariant = 0x40;
    }
    mask = 0x10;
    if (actor->base.statusFlags & mask) {
        SceneActorChase_SetControl(actor, Slime_UpdateStill, 0x2C);
        Slime_EmitScatteredReactionBurst(actor);
        Slime_EmitScatteredReactionBurst(actor);
        Sound_PlaySong(0x3C2);
    }
}

void Slime_UpdateStill(struct ChaseActor *actor)
{
    struct SceneActorAnimationSequenceDrawable *d = actor->base.drawable;
    if (Slime_TakePrepared(actor)) {
        d->sprite.palette = 0x40;
        d->sprite.paletteVariant = 0x40;
        d->sprite.flags &= ~2;
        SceneActorChase_ClearMotionFlagComponent(actor, 0x100);
    }
    if (actor->base.restart) {
        actor->base.field2C2 = 0x40;
        SceneActorChase_SetControl(actor, SceneActorControl_OnIdle, 0x10);
        SceneActorChase_SetState(actor, 0, 0, 2);
    }
}

/* SceneActorControl_CompleteScriptedResponse_0809F368: SceneActorControl_CompleteScriptedResponse with other constants (ported by relatives.py, fixed by hand; provisional name) */

void *Bestiary_GetEntryForActor(u32, u32);

void Bestiary_UnlockEntry(void *);

void SceneActorBehavior_CallTerrainScript(struct RoomActor *);

void SceneActorPlacement_PlayCategoryEventSound(struct ChaseActor *);

u8 Sprite_BlinkUntilExpired(void *, s32, s32, s32);

void Slime_RunDefeatScript(struct ChaseActor *actor, s32 frame)
{
    s32 values[8];
    struct ScriptUtilityArguments arguments;
    u16 script;
    u32 *stateFlags;
    if (SceneActorControl_TakeFlag(&actor->base.field2B2)) {
        SceneActorAnimation_SelectProfileEntry((void *)actor, 10);
        SceneActorPlacement_PlayCategoryEventSound((void *)actor);
        Slime_EmitDeathDebris(actor);
        {
            u16 *p = &actor->base.motionFlags;
            u32 mask = ~0x38;
            *p = mask & *p;
        }
    }
    if (SceneActorContact_ReadActorFlags(actor, 0x8000, &stateFlags))
        return;

    if (Sprite_BlinkUntilExpired(actor->base.drawable, frame, 10, 30)) {
        actor->base.stepDone = 1;
        script = (u16)actor->base.script1F8;
        if (script) {
            values[0] = actor->base.body.position.x;
            values[1] = actor->base.body.position.y;
            values[2] = actor->base.body.position.z;
            values[3] = actor->base.parameters1FC[0];
            values[4] = actor->base.parameters1FC[1];
            values[5] = actor->base.parameters1FC[2];
            values[6] = actor->base.hitOwners;
            values[7] = actor->base.body.id;
            arguments.count = 8;
            arguments.values = values;
            Script_Call(script, &arguments);
        }
        if (!(SceneActorChase_QueryMotionFlagComponent(actor, 0x4000)) &&
            (u32)(actor->base.category - 11) > 2)
            Bestiary_UnlockEntry(
                Bestiary_GetEntryForActor(actor->base.category, actor->base.paletteOffset));
        if (!(SceneActorChase_Bits32(stateFlags, 0x400)))
            SceneActorBehavior_CallTerrainScript((void *)actor);
        {
            void *callback = (void *)SceneActorControl_OnIdle;
            u32 kind;
            kind = 16;
            actor->base.field2B2 = 1;
            actor->base.stepDone = 0;
            actor->base.controlKind = kind;
            actor->base.target = callback;
            actor->base.field2C8 = 0;
            {
                u32 *flags = &actor->base.pendingControlFlags;
                u32 mask = ~1;
                *flags &= mask;
            }
            actor->base.pendingControl = 0;
        }
        SceneActorTerrain_EnterState(actor, 1, 0, 2);
    } else {
        if (SceneActorVisibility_IsHidden(actor))
            ActorShadow_Disable(actor->base.shadow);
        else
            ActorShadow_Enable(actor->base.shadow);
    }
}

void SceneActorBehavior_CallActionScript(u8 *);


void Slime_UpdateActionScript(struct ChaseActor *obj)
{
    struct Slime *context = obj->base.context;

    if (SceneActorControl_TakeFlag(&obj->base.field2B1)) {
        obj->base.body.direction = obj->base.tracking.heading;
        SceneActorBehavior_CallActionScript((u8 *)obj);
    }
    if (obj->base.stepDone) {
        u32 flag = 4;
        u16 *h = &context->flags;
        flag |= *h;
        *h = flag;
    } else {
        ++obj->base.counter2AC;
    }
}

extern void *gUnk_08605D14[];

void Slime_SetSecondaryStateTable(struct ChaseActor *actor)
{
    actor->base.alternateBehaviorCallbacks = gUnk_08605D14;
}

s32 Math_Atan2(s32, s32);

s32 Callback_InvokeActor(void *, void *);

void Anim_SetFrameDurationScale(void *, u32);



static inline void Slime_SetApproachControl(struct ChaseActor *a, void *cb, u32 kind)
{
    a->base.field2B2 = 1;
    a->base.stepDone = 0;
    a->base.controlKind = kind;
    a->base.target = cb;
    a->base.field2C8 = 0;
    {
        u32 *flags = &a->base.pendingControlFlags;
        u32 mask = ~1;
        *flags &= mask;
    }
    a->base.pendingControl = 0;
}

static inline u8 Slime_ApproachWanderTarget(struct ChaseActor *actor,
                                            struct SceneActorMotionSubject *target, s32 speed)
{
    struct SceneActorBody *motion = &actor->base.body;
    s16 dx = target->position.x - motion->position.x;
    s16 dz = target->position.z - motion->position.z;
    if ((dx < 0 ? -dx : dx) <= 32 && (dz < 0 ? -dz : dz) <= 32)
        return 1;
    if (dx || dz) {
        motion->direction = Math_Atan2(dx, dz);
        motion->motion.x +=
            TentacleSpread_Shift(gUnk_086149C4[(motion->direction + 64) & 255] * speed, 12);
        motion->motion.z += TentacleSpread_Shift(gUnk_086149C4[motion->direction] * speed, 12);
    }
    return 0;
}

static inline void Slime_SetMotionScale(struct ChaseActor *actor)
{
    u16 *scale = SceneActorChase_PrepareAnimationScale(actor, 0xA02F);
    if (actor->base.drawableKind == 0)
        Anim_SetDurationScale(
            &((struct SceneActorAnimationSequenceDrawable *)actor->base.drawable)->sequence,
            *scale);
    else
        Anim_SetFrameDurationScale(
            &((struct SceneActorAnimationTileDrawable *)actor->base.drawable)->tile, *scale);
}

// confidence=low: moves toward a saved waypoint, then re-selects the route.

void Slime_ApproachSavedTarget(struct ChaseActor *actor)
{
    struct Slime *context = actor->base.context;
    s32 speed;
    if (SceneActorControl_TakeFlag(&actor->base.field2B1)) {
        Slime_SetApproachControl(actor, actor->base.onAlternateEntry, 1);
        Slime_SelectWanderTarget(actor, actor->base.body.direction);
        actor->base.field2C2 = 0x40;
        return;
    }
    Callback_InvokeActor(actor, actor->base.contactQuery.opaque);
    speed = Callback_InvokeActor(actor, actor->base.animationQuery.opaque);
    Slime_SetMotionScale(actor);
    if (Slime_ApproachWanderTarget(actor, &context->wanderTarget, speed))
        Slime_SelectWanderTarget(actor, actor->base.body.direction);
    SceneActorChase_SetActorFlags(actor, 0x2000);
    actor->base.counter2AC++;
}

void Slime_RestoreSavedTarget(struct ChaseActor *actor)
{
    if (SceneActorControl_TakeStateEntry(actor))
        SceneActorChase_SetControl(actor, actor->base.onStanding, 0);
    Callback_InvokeActor(actor, actor->base.contactQuery.opaque);
    actor->base.counter2AC++;
}

void Slime_RunUpdateThenWait(struct ChaseActor *actor)
{
    u8 result;
    if (actor->base.field2B1)
        actor->base.field2B1 = 0;
    result = Callback_InvokeActor(actor, actor->base.contactQuery.opaque);
    if (!result) {
        SceneActorChase_SetControl(actor, Slime_UpdateReturnToIdle, 0x27);
        SceneActorChase_SetState(actor, 3, 1, 0);
    }
    actor->base.counter2AC++;
}

extern void *gUnk_08605D18[];

void Slime_SetPrimaryStateTable(struct ChaseActor *actor)
{
    actor->base.callbacks284 = gUnk_08605D18;
}

void SceneActorCollision_InitBody(u8 *, struct SlimeCollisionVector *,
                                  struct SlimeCollisionVector *, void *, u8 *);

void SceneActorCollision_InitAttack(u8 *, struct SlimeCollisionVector *,
                                    struct SlimeCollisionVector *, void *, u32, u8 *);

void Slime_InitColliders(struct ChaseActor *actor)
{
    struct SlimeCollisionVector s1, s2;
    s1.xy.first = 0x40;
    s1.xy.second = 0x80;
    s1.zw.first = 0x40;
    *(u32 *)&s2.xy = 0x800000;
    *(u32 *)&s2.zw &= 0xFFFF0000;
    SceneActorCollision_InitBody(actor, &s1, &s2, Slime_ResolveBodyContact, actor);
    ((struct ContactPosition *)&s1)->x = 0x40;
    ((struct ContactPosition *)&s1)->y = 0x64;
    ((struct ContactPosition *)&s1)->z = 0x40;
    ((struct ContactPosition *)&s2)->x = 0;
    ((struct ContactPosition *)&s2)->y = 0x80;
    ((struct ContactPosition *)&s2)->z = 0;
    SceneActorCollision_InitAttack(actor, &s1, &s2, Slime_ApplyBodyHit, 0, actor);
}

// confidence=low: clears context timers, selects idle animation, and advances its first frame.

void Slime_ResetIdlePlayback(struct ChaseActor *actor)
{
    struct Slime *context = actor->base.context;
    context->flags = 0;
    context->enrageCount = 0;
    SceneActorAnimation_SelectDirectionalEntry(actor, 0);
    {
        struct SceneActorControlMotionDrawable *drawable = actor->base.drawable;
        DisplaySequence_StepState(drawable, &drawable->sequence);
    }
}

/* Slime_OnUnusedEvent: Empty function in the Slime implementation; event role is tentative */

void Slime_OnUnusedEvent(void)
{
}

void Slime_InitProfile(struct ChaseActor *actor)
{
    actor->base.profileId = SceneActorProfiles_FindOrAdd(0x16, 0);
}

s32 Mod(s32, s32);

extern const u16 gUnk_08605D58[];

// confidence=low: emits the profile's six-frame movement pulse at animation frame boundaries.

s32 Slime_GetAnimationMovePulse(struct ChaseActor *actor)
{
    struct DisplaySequenceState *sequence =
        &((struct SceneActorControlMotionDrawable *)actor->base.drawable)->sequence;
    s32 result;
    if (sequence->elapsed)
        result = 0;
    else {
        MonsterInit_SetStatusFlags(actor, 0x10000);
        result = Mod(sequence->index, 6);
        {
            const u16 *table = gUnk_08605D58;
            result = table[result];
        }
    }
    return result;
}

u8 SceneActorTerrain_CanTraverseCell(u32, u32, u32, u32);

static inline u16 Slime_RandomDirection(void)
{
    u32 index = (gRandomIndex + 1) & 1023;
    gRandomIndex = index;
    index *= 2;
    return *(const u16 *)(RANDOM_TABLE_ADDR + index);
}

static inline s32 Slime_ForwardCell(struct ChaseActor *actor, s32 steps)
{
    s32 cell = actor->base.terrainCell;
    struct ContactPosition *position = &actor->base.body.position;
    struct EngineMap **cachedMapSlot = &gTerrainMap;
    u32 height = SceneActorChase_HeightAt(cell, position);
    u32 shape = SceneActorChase_CellProperties(cell)->attributes >> 4;
    s32 direction = Ghoul_QuarterDirection(actor->base.body.direction + 32) & 3;
    s32 next;
    s32 i;
    struct EngineMap **completionMapSlot;
    switch (direction) {
        case 0:
            next = cell + 1;
            i = 0;
            if (i < steps)
                do {
                    if (!SceneActorTerrain_CanTraverseCell(cell, next, height, shape))
                        break;
                    next++;
                } while (++i < steps);
            return next - 1;
        case 1:
            next = cell + Ghoul_MapWidth(cachedMapSlot);
            i = 0;
            completionMapSlot = &gTerrainMap;
            if (i < steps)
                do {
                    if (!SceneActorTerrain_CanTraverseCell(cell, next, height, shape))
                        goto south;
                    next += Ghoul_MapWidth(cachedMapSlot);
                } while (++i < steps);
            return next - Ghoul_MapWidth(completionMapSlot);
        case 2:
            next = cell - 1;
            i = 0;
            if (i < steps)
                do {
                    if (!SceneActorTerrain_CanTraverseCell(cell, next, height, shape))
                        break;
                    next--;
                } while (++i < steps);
            return next + 1;
        case 3:
            next = cell - Ghoul_MapWidth(cachedMapSlot);
            i = 0;
            completionMapSlot = &gTerrainMap;
            if (i < steps)
                do {
                    if (!SceneActorTerrain_CanTraverseCell(cell, next, height, shape))
                        goto north;
                    next -= Ghoul_MapWidth(cachedMapSlot);
                } while (++i < steps);
            return next + Ghoul_MapWidth(completionMapSlot);
        default:
            goto home;
    }
south:
    return next - Ghoul_MapWidth(cachedMapSlot);
north:
    return next + Ghoul_MapWidth(cachedMapSlot);
home:
    return actor->base.terrainCell;
}

void Slime_SelectWanderTarget(struct ChaseActor *actor, u32 direction)
{
    struct ContactPosition center;
    struct Slime *context = actor->base.context;
    s32 tries, chosen;
    actor->base.body.direction = Slime_RandomDirection();
    /* The selected exit preserves the original loop rotation in agbcc. */
    tries = 0;
    do {
        chosen = Slime_ForwardCell(actor, 2);
        if (chosen != actor->base.terrainCell)
            goto selected;
        actor->base.body.direction += 64;
        tries++;
    } while (tries <= 3);
selected:
    actor->base.body.direction = direction;
    SceneActorTerrain_GetCenter(chosen, &center);
    context->wanderTarget.position = center;
}

static inline void Slime_SetFlags2D6(struct ChaseActor *actor, u32 bits)
{
    u32 mask = bits;
    u16 *flags = &actor->base.reactionFlags;
    mask |= *flags;
    *flags = mask;
}

u32 Slime_TryEnrage(struct ChaseActor *actor)
{
    struct Slime *q = actor->base.context;
    s32 dy;
    actor->base.field2C2 = 0x40;
    if (!MonsterInit_TestTargetingFlags(&actor->base.tracking, 2))
        return 0;
    dy = actor->base.body.position.y -
         ((struct SceneActorChasePlayer *)gPlayers[0])->target.position.y;
    if (dy < 0)
        dy = -dy;
    if (dy > 0x200)
        return 0;
    if (actor->base.tracking.distance > actor->base.tracking.limit)
        return 0;
    if (actor->base.health < q->halfHealth || Slime_HasContextFlag(q, 8)) {
        Slime_SetFlags2D6(actor, 0x80);
        {
            struct SceneActorChaseTracking *p = &actor->base.tracking;
            u32 mask = 0x10;
            mask |= p->flags;
            p->flags = mask;
        }
        actor->base.field2C2 = 0x20;
        q->enrageCount++;
    } else {
        Slime_SetFlags2D6(actor, 0x100);
    }
    return 1;
}

void Slime_FollowPlayer(struct ChaseActor *actor)
{
    struct Slime *q = actor->base.context;
    struct SceneActorChasePlayer *p;
    u32 zero;
    struct ContactPosition *dst;
    struct SceneActorReactionMotion *m =
        (struct SceneActorReactionMotion *)&actor->base.tracking.target;
    {
        u32 v = 8;
        u32 f = m->flags;
        zero = 0;
        v |= f;
        m->flags = v;
    }
    dst = &q->wanderTarget.position;
    p = (struct SceneActorChasePlayer *)gPlayers[0];
    *dst = p->target.position;
    if (q->enrageCount >= q->launchThreshold) {
        SceneActorChase_SetControl(actor, Slime_UpdateShake, 0x2A);
        SceneActorChase_SetState(actor, 3, zero, zero);
    }
}

void Slime_OnInterrupt(struct ChaseActor *actor)
{
    u8 kind = actor->base.controlKind;
    if (kind != 0x26 && kind != 0x22 && kind != 0x23 && kind != 0x24) {
        SceneActorChase_SetControl(actor, Slime_StartRandomWait, 0x26);
        SceneActorChase_SetState(actor, 0, 0, 0xF);
    }
}

void Slime_ReadSplitScript(struct ChaseActor *actor)
{
    struct Slime *context = actor->base.context;
    s32 a = Script_SeekToKeyword('s');
    s32 b;
    if (a) {
        if (Script_GetPc())
            a = Script_GetValue();
        else
            a = 0;
        context->splitScript = a;
        if (Script_GetPc())
            b = Script_GetValue();
        else
            b = 0;
        context->splitParameter = b;
    } else {
        context->splitScript = a;
        context->splitParameter = a;
    }
}

/* SceneActorControl_CompleteScriptedResponse_0809F368: SceneActorControl_CompleteScriptedResponse with other constants (ported by relatives.py, fixed by hand; provisional name) */

static inline s32 Slime_ScriptFixed12(s32 n)
{
    return n >= 0 ? n >> 12 : -((-n) >> 12);
}

static inline void Slime_ScriptPolarMotion(struct ContactPosition *out, u32 angle, s32 speed)
{
    out->x = Slime_ScriptFixed12(gUnk_086149C4[(angle + 64) & 255] * speed);
    out->y = 0;
    out->z = Slime_ScriptFixed12(gUnk_086149C4[angle] * speed);
}

// confidence=low: invokes a pending Slime script with motion and variant arguments, then resets control.

void Slime_RunPendingActionScript(struct ChaseActor *actor)
{
    s32 values[10];
    struct ContactPosition vector;
    struct ScriptUtilityArguments args;
    struct Slime *context = actor->base.context;
    if (context->splitScript) {
        values[0] = actor->base.body.position.x;
        values[1] = actor->base.body.position.y;
        values[2] = actor->base.body.position.z;
        values[3] = actor->base.health;
        Slime_ScriptPolarMotion(&vector, actor->base.reactionDirection, 12);
        values[4] = vector.x;
        values[5] = 24;
        values[6] = vector.z;
        values[7] = actor->base.paletteOffset;
        values[8] = context->splitParameter;
        values[9] = (u16)actor->base.defaultHealth;
        args.count = 10;
        args.values = values;
        Script_Call(context->splitScript, &args);
        Slime_EmitDeathDebris(actor);
        context->splitScript = 0;
        {
            u32 flags = 8;
            u16 *p = &context->flags;
            flags |= *p;
            *p = flags;
        }
        SceneActorChase_SetControl(actor, SceneActorControl_OnIdle, 0x10);
        SceneActorTerrain_EnterState(actor, 0, 0, 2);
        {
            u16 *p = &actor->base.reactionFlags;
            u32 mask = ~0x980;
            *p = mask & *p;
        }
    }
}

u32 Slime_GetRandomVariantFlags(struct ChaseActor *actor)
{
    s32 v;
    u32 r;
    if (actor->base.paletteOffset != 3)
        return 0;
    gRandomIndex = (gRandomIndex + 1) & 0x3ff;
    v = *(u8 *)(((u8 *)gRandomTable) + gRandomIndex * 2);
    r = 0x200004;
    if (v <= 0xb3)
        r = 0x400008;
    return r;
}

// confidence=low: emits three debris particles during the Slime death reaction.

void sub_08127590(void *, void *, u32, u32, s32, s32, s32, s32, s32, s32, s32, s32, s32, s32, s32);

static inline s32 Slime_DebrisFixed12(s32 x)
{
    if (x >= 0)
        return x >> 12;
    return -((-x) >> 12);
}

static inline u16 Slime_DebrisRandom(void)
{
    const u16 *table = gRandomTable;
    u32 index = (gRandomIndex + 1) & 1023;
    gRandomIndex = index;
    index *= 2;
    return *(const u16 *)((u32)table + index);
}

static inline void Slime_DebrisPolarMotion(s32 angle, struct ContactPosition *v, s32 radius,
                                           u32 mask)
{
    v->x = Slime_DebrisFixed12(gUnk_086149C4[(angle + 64) & mask] * radius);
    v->y = 0;
    v->z = Slime_DebrisFixed12(gUnk_086149C4[angle & 255] * radius);
}

// confidence=low: death burst of three debris particles.

#define BURST(v)                                                                                   \
    {                                                                                              \
        u32 mask = 255;                                                                            \
        s32 angle;                                                                                 \
        angle = Slime_DebrisRandom() & 255;                                                        \
        Slime_DebrisPolarMotion(angle, &v, (Slime_DebrisRandom() & 15) + 6, mask);                 \
    }

static inline void Slime_DebrisPalette(struct ChaseActor *actor, u16 **bank, u16 **offset)
{
    *bank = &actor->base.basePaletteBank;
    *offset = &actor->base.paletteOffset;
}

void Slime_EmitDeathDebris(struct ChaseActor *actor)
{
    /* The actor, then (from the first emission on) the actor's position at 0x5C. */
    void *base = actor;
    struct ContactPosition v;
    u16 *b;
    u16 *a;
    BURST(v)
    v.y = 0x30;
    {
        Slime_DebrisPalette(base, &a, &b);
    }
    {
        u32 sum = *a + *b;
        base = &((struct ChaseActor *)base)->base.body.position;
        sub_08127590(base, &v, 0xB96C, 0xA929, 6, 0x18, 0x200, 0, sum, 0, 0, 0, 0, 0, 0);
    }
    BURST(v)
    v.y = 0x25;
    {
        u32 sum = *a + *b;
        sub_08127590(base, &v, 0xB96C, 0xA929, 7, 0x18, 0x200, 0, sum, 0, 0, 0, 0, 0, 0);
    }
    BURST(v)
    v.y = 0x20;
    {
        u32 sum = *a + *b;
        sub_08127590(base, &v, 0xB96C, 0xA929, 7, 0x18, 0x200, 0, sum, 1, 0, 0, 0, 0, 0);
    }
}

static inline u16 Slime_TargetBurstRandom(void)
{
    const u16 *table = gRandomTable;
    u32 index = (gRandomIndex + 1) & 1023;
    gRandomIndex = index;
    index *= 2;
    return *(const u16 *)((u32)table + index);
}

static inline s32 Slime_TargetBurstFixed12(s32 n)
{
    return n >= 0 ? n >> 12 : -((-n) >> 12);
}

static inline void Slime_TargetBurstPolarMotion(struct ContactPosition *v, s32 angle, s32 radius)
{
    v->x = Slime_TargetBurstFixed12(gUnk_086149C4[(angle + 64) & 255] * radius);
    v->y = 0;
    v->z = Slime_TargetBurstFixed12(gUnk_086149C4[angle & 255] * radius);
}


static inline u16 *Slime_TargetBurstPalette(struct ChaseActor *actor)
{
    return &actor->base.basePaletteBank;
}

static inline u16 *Slime_TargetBurstOffset(struct ChaseActor *actor)
{
    return &actor->base.paletteOffset;
}

static inline u16 *Slime_TargetBurstPower(struct ChaseActor *actor)
{
    return &actor->base.attacks.first[1];
}

static inline u16 *Slime_TargetBurstParameter(struct ChaseActor *actor)
{
    return &actor->base.attacks.second[1];
}

static inline u32 *Slime_TargetBurstFlags(struct ChaseActor *actor)
{
    return &actor->base.attacks.flags[1];
}

// confidence=low: launches three randomized reaction projectiles toward the current target.

void Slime_EmitTargetedReactionBurst(struct ChaseActor *input)
{
    struct ChaseActor *actor = input;
    struct ContactPosition velocity;
    s32 angle;
    u32 variant = Slime_GetRandomVariantFlags(actor);
    s16 jitter = Slime_TargetBurstRandom() & 15;
    Chase_Delta(&velocity, &actor->base.body.position,
                           &actor->base.tracking.target->position);
    angle = Math_Atan2(velocity.x, velocity.z);
    Slime_TargetBurstPolarMotion(&velocity, angle + jitter, (Slime_TargetBurstRandom() & 7) + 6);
    velocity.y = 48;
    {
        u16 *palette;
        u16 *offset;
        u16 *power;
        u16 *parameter;
        u32 *flags;
        u16 *value;
        struct ContactPosition *position;
        u32 sum, attack, response, bits, v;
        palette = Slime_TargetBurstPalette(actor);
        offset = Slime_TargetBurstOffset(actor);
        sum = *palette + *offset;
        power = Slime_TargetBurstPower(actor);
        attack = *power;
        parameter = Slime_TargetBurstParameter(actor);
        response = *parameter;
        flags = Slime_TargetBurstFlags(actor);
        bits = *flags | variant;
        value = &actor->base.attacks.parameter[1];
        v = *value;
        position = &actor->base.body.position;
        sub_08127590(position, &velocity, 0xB96C, 0xA929, 6, 24, 0, 0, sum, 0, 0, attack, response,
                     bits, v);
    }
    jitter = Slime_TargetBurstRandom() & 15;
    Slime_TargetBurstPolarMotion(&velocity, angle + jitter, (Slime_TargetBurstRandom() & 7) + 6);
    velocity.y = 37;
    {
        u16 *palette;
        u16 *offset;
        u16 *power;
        u16 *parameter;
        u32 *flags;
        u16 *value;
        struct ContactPosition *position;
        u32 sum, attack, response, bits, v;
        palette = Slime_TargetBurstPalette(actor);
        offset = Slime_TargetBurstOffset(actor);
        sum = *palette + *offset;
        power = Slime_TargetBurstPower(actor);
        attack = *power;
        parameter = Slime_TargetBurstParameter(actor);
        response = *parameter;
        flags = Slime_TargetBurstFlags(actor);
        bits = *flags | variant;
        value = &actor->base.attacks.parameter[1];
        v = *value;
        position = &actor->base.body.position;
        sub_08127590(position, &velocity, 0xB96C, 0xA929, 7, 24, 0, 0, sum, 0, 0, attack, response,
                     bits, v);
    }
    jitter = Slime_TargetBurstRandom() & 15;
    Slime_TargetBurstPolarMotion(&velocity, angle + jitter, (Slime_TargetBurstRandom() & 7) + 6);
    velocity.y = 32;
    {
        u16 *palette;
        u16 *offset;
        u16 *power;
        u16 *parameter;
        u32 *flags;
        u16 *value;
        struct ContactPosition *position;
        u32 sum, attack, response, bits, v;
        palette = Slime_TargetBurstPalette(actor);
        offset = Slime_TargetBurstOffset(actor);
        sum = *palette + *offset;
        power = Slime_TargetBurstPower(actor);
        attack = *power;
        parameter = Slime_TargetBurstParameter(actor);
        response = *parameter;
        flags = Slime_TargetBurstFlags(actor);
        bits = *flags | variant;
        value = &actor->base.attacks.parameter[1];
        v = *value;
        position = &actor->base.body.position;
        sub_08127590(position, &velocity, 0xB96C, 0xA929, 6, 24, 0, 0, sum, 1, 0, attack, response,
                     bits, v);
    }
}

static inline u32 Slime_RandomSample(void)
{
    const u16 *table = gRandomTable;
    u32 index = (gRandomIndex + 1) & 1023;
    gRandomIndex = index;
    return table[index];
}

static inline s32 Slime_TruncateFixed12(s32 n)
{
    return n >= 0 ? n >> 12 : -((-n) >> 12);
}

static inline void Slime_SetPolarMotion(struct ContactPosition *v, s32 angle, s32 radius)
{
    v->x = Slime_TruncateFixed12(gUnk_086149C4[(angle + 64) & 255] * radius);
    v->y = 0;
    v->z = Slime_TruncateFixed12(gUnk_086149C4[angle & 255] * radius);
}


static inline u16 *Slime_PaletteBase(struct ChaseActor *actor)
{
    return &actor->base.basePaletteBank;
}

static inline u16 *Slime_PaletteOffset(struct ChaseActor *actor)
{
    return &actor->base.paletteOffset;
}

static inline u16 *Slime_AttackPower(struct ChaseActor *actor)
{
    return &actor->base.attacks.first[1];
}

static inline u16 *Slime_AttackResponse(struct ChaseActor *actor)
{
    return &actor->base.attacks.second[1];
}

static inline u32 *Slime_AttackFlags(struct ChaseActor *actor)
{
    return &actor->base.attacks.flags[1];
}

static inline u16 *Slime_AttackParameter(struct ChaseActor *actor)
{
    return &actor->base.attacks.parameter[1];
}

static inline struct ContactPosition *Slime_Position(struct ChaseActor *actor)
{
    return &actor->base.body.position;
}

/* Emit three reaction projectiles with independent random headings. */

void Slime_EmitScatteredReactionBurst(struct ChaseActor *input)
{
    struct ChaseActor *actor = input;
    struct ContactPosition velocity;
    s32 angle;
    u32 variant = Slime_GetRandomVariantFlags(actor);
    s32 jitter = Slime_RandomSample() & 15;
    Chase_Delta(&velocity, &actor->base.body.position,
                            &actor->base.tracking.target->position);
    angle = Slime_RandomSample() & 255;
    Slime_SetPolarMotion(&velocity, angle + jitter, (Slime_RandomSample() & 7) + 12);
    velocity.y = 48;
    {
        u16 *palette;
        u16 *offset;
        u16 *power;
        u16 *parameter;
        u32 *flags;
        u16 *value;
        struct ContactPosition *position;
        u32 sum, attack, response, bits, v;
        palette = Slime_PaletteBase(actor);
        offset = Slime_PaletteOffset(actor);
        sum = *palette + *offset;
        power = Slime_AttackPower(actor);
        attack = *power;
        parameter = Slime_AttackResponse(actor);
        response = *parameter;
        flags = Slime_AttackFlags(actor);
        bits = *flags | variant;
        value = Slime_AttackParameter(actor);
        v = *value;
        position = Slime_Position(actor);
        sub_08127590(position, &velocity, 0xB96C, 0xA929, 6, 24, 0, 0, sum, 0, 0, attack, response,
                     bits, v);
    }
    jitter = Slime_RandomSample() & 15;
    angle = Slime_RandomSample() & 255;
    Slime_SetPolarMotion(&velocity, angle + jitter, (Slime_RandomSample() & 7) + 12);
    velocity.y = 37;
    {
        u16 *palette;
        u16 *offset;
        u16 *power;
        u16 *parameter;
        u32 *flags;
        u16 *value;
        struct ContactPosition *position;
        u32 sum, attack, response, bits, v;
        palette = Slime_PaletteBase(actor);
        offset = Slime_PaletteOffset(actor);
        sum = *palette + *offset;
        power = Slime_AttackPower(actor);
        attack = *power;
        parameter = Slime_AttackResponse(actor);
        response = *parameter;
        flags = Slime_AttackFlags(actor);
        bits = *flags | variant;
        value = Slime_AttackParameter(actor);
        v = *value;
        position = Slime_Position(actor);
        sub_08127590(position, &velocity, 0xB96C, 0xA929, 7, 24, 0, 0, sum, 0, 0, attack, response,
                     bits, v);
    }
    jitter = Slime_RandomSample() & 15;
    angle = Slime_RandomSample() & 255;
    Slime_SetPolarMotion(&velocity, angle + jitter, (Slime_RandomSample() & 7) + 6);
    velocity.y = 32;
    {
        u16 *palette;
        u16 *offset;
        u16 *power;
        u16 *parameter;
        u32 *flags;
        u16 *value;
        struct ContactPosition *position;
        u32 sum, attack, response, bits, v;
        palette = Slime_PaletteBase(actor);
        offset = Slime_PaletteOffset(actor);
        sum = *palette + *offset;
        power = Slime_AttackPower(actor);
        attack = *power;
        parameter = Slime_AttackResponse(actor);
        response = *parameter;
        flags = Slime_AttackFlags(actor);
        bits = *flags | variant;
        value = Slime_AttackParameter(actor);
        v = *value;
        position = Slime_Position(actor);
        sub_08127590(position, &velocity, 0xB96C, 0xA929, 6, 24, 0, 0, sum, 1, 0, attack, response,
                     bits, v);
    }
}

void Sound_PlaySong(u32);

static inline void Slime_ClearPendingFlags(u16 *q, u32 m)
{
    *q = m & *q;
}

static inline u8 Slime_TestPendingFlag(u16 mask, const u16 *field)
{
    if (*field & mask)
        return TRUE;
    return FALSE;
}

void Slime_PlayPendingSound(struct ChaseActor *actor)
{
    struct Slime *context = actor->base.context;
    if (Slime_TestPendingFlag(0x10, &context->flags)) {
        Sound_PlaySong(0x3C2);
        Slime_ClearPendingFlags(&context->flags, ~0x10);
    }
}

void SceneActorMotion_ArmPrimaryCollider(struct SceneActorMotionView *actor)
{
    s32 health;

    actor->effectDirection = 0;
    actor->payload2E0 = 1;
    health = actor->health;
    actor->primaryCollider.cost = health;
    actor->result2E4 = health;
    {
        u16 *field = &actor->primaryCollider.responseParameter;
        *field = 40;
    }
    actor->parameter2D8 = 0;
    actor->field2DC = 0;
    actor->mode2E6 = 1;
    SceneActorChase_SetActorFlags(actor, 0x2000);
}

static inline void Slime_ClearMotionBits(u16 *p, u32 mask)
{
    *p = mask & *p;
}

static inline void Slime_ClearReactionBits(u16 *p, u32 mask)
{
    ((struct SlimeHalfFlags *)p)->bits &= mask;
}

static inline u8 Slime_TestReactionFlag(struct Slime *s, u32 mask)
{
    if (s->flags & mask)
        return 1;
    return 0;
}



static inline u32 Slime_CollisionDamageFlags(struct ContactCollider *p)
{
    return p->statRewardFlags;
}

static inline u32 Slime_MaskedDamage(struct ChaseActor *actor, u32 flags, u32 mask)
{
    return actor->base.hitReactionFlags & flags & mask;
}

static inline u32 Slime_DamageFlags(u32 *flags)
{
    return *flags;
}

// confidence=low: reaction callback chooses split/idle/recovery states after damage.

u32 Slime_ResolveDamageReaction(struct ChaseActor *actor)
{
    struct Slime *context = actor->base.context;
    u16 *motionFlags;
    u32 damage;
    if (actor->base.reactionSoundVariant == 0)
        Sound_PlaySong(0x211);
    else
        Sound_PlaySong(0x3bf);
    if (actor->base.paletteOffset == 3)
        Slime_EmitTargetedReactionBurst(actor);
    Slime_ClearMotionBits(&actor->base.motionFlags, ~0x20);
    context->enrageCount += 80;
    damage = Slime_MaskedDamage(actor, Slime_CollisionDamageFlags(&actor->base.colliderA), 0x7f);
    motionFlags = &actor->base.motionFlags;
    if (damage == 0 && context->splitScript) {
        if (actor->base.health < context->halfHealth)
            Slime_RunPendingActionScript(actor);
        else {
            u32 index = (gRandomIndex + 1) & 1023;
            gRandomIndex = index;
            {
                s32 random = *(const u8 *)(RANDOM_TABLE_ADDR + index * 2);
                if (random <= 24)
                    Slime_RunPendingActionScript(actor);
            }
        }
    }
    if ((Slime_DamageFlags(&actor->base.hitReactionFlags) & 0x40000)) {
        Slime_ClearReactionBits(&actor->base.reactionFlags, ~0x980);
        Chase_SetCtl(actor, actor->base.onReaction, 0x12);
        Chase_SetState(actor, 3, 0, 0);
    } else {
        if (SceneActorTurn_Flags(motionFlags, 0x100))
            return 0;
        if (actor->base.health < context->halfHealth || Slime_TestReactionFlag(context, 8)) {
            Chase_SetCtl(actor, SceneActorControl_OnIdle, 0x10);
            Chase_SetState(actor, 0, 0, 2);
            Slime_ClearReactionBits(&actor->base.reactionFlags, ~0x980);
        } else if ((*(u32 *)&actor->base.state2A8 & 0xffffff) != 0) {
            Chase_SetCtl(actor, actor->base.onStanding, 0);
            Chase_SetState(actor, 0, 0, 0);
        }
    }
    return 1;
}

u32 Slime_ProcessPendingStateChange(struct ChaseActor *actor)
{
    if (Slime_TestPendingFlag(0x100, &actor->base.reactionFlags)) {
        Slime_OnInterrupt(actor);
        Slime_ClearPendingFlags(&actor->base.reactionFlags, ~0x100);
    }
    return 1;
}

s32 SceneActorMotion_RefreshGroundHeight(struct Slime *slime)
{
    struct SceneActorMotionView *actor = (void *)slime;

    if ((s32)slime->base.groundHeight - slime->previousGround > 0x1FF) {
        s32 cell = SceneActorTerrain_GetCellIndex(&slime->base.body.position);
        if (TerrainModifiers_FindCellOverride(cell, 1)) {
            SceneActorMotion_ArmPrimaryCollider(actor);
            actor->groundHeight = slime->previousGround;
        }
    }
    {
        u16 value = actor->groundHeight;
        slime->previousGround = value;
    }
    return 1;
}

void SceneActorControl_DispatchControl(u8 *);

u32 Slime_Update(struct ChaseActor *actor)
{
    u32 i;
    void **t;
    Slime_ProcessPendingStateChange(actor);
    i = actor->base.state2A8;
    t = actor->base.stateCallbacks;
    Callback_InvokeActor(actor, t[i]);
    SceneActorControl_DispatchControl(actor);
    SceneActorMotion_RefreshGroundHeight(actor);
    return 1;
}

void SceneActor_UpdateActiveCallbacks(u8 *);

u32 Slime_OnMessage(struct ChaseActor *actor)
{
    actor->base.tracking.target = &((struct SceneActorChasePlayer *)gPlayers[0])->target;
    SceneActor_UpdateActiveCallbacks((void *)actor);
    return 0;
}

void Sprite_Remove(void *);

void Anim_Remove(void *);

void SceneMessages_UnregisterReceiver(void *);

void Shadow_Destroy(void *);

void TerrainOverlay_QueueRemoveActor(void *);

void SceneBody_Unregister(void *);

void Collider_Remove(void *);

void SceneActorRegistry_UnlinkActor(void *);

u32 Slime_Destroy(struct ChaseActor *actor)
{
    u8 *p = actor->base.drawable;
    u8 *q;
    u32 m;
    u32 v;
    if (actor->base.drawableKind == 0)
        Sprite_Remove(p);
    else
        Anim_Remove(&((struct SceneActorAnimationTileDrawable *)p)->tile);
    Heap_Free(p);
    m = 0x100;
    if (SceneActorChase_TestField(&actor->base.placementFlags, m))
        SceneMessages_UnregisterReceiver(actor->base.receiver);
    {
        u8 *r = actor->base.shadow;
        if (*r == 1)
            Shadow_Destroy(r);
    }
    m = 0x2000000;
    v = actor->base.statusFlags & m;
    q = (void *)&actor->base.body;
    if (v == 0)
        TerrainOverlay_QueueRemoveActor(q);

    SceneBody_Unregister(q);
    Collider_Remove(&actor->base.colliderA);
    SceneActorRegistry_UnlinkActor(actor);
    return 0;
}

void SceneActorRegistry_SetObjectContext(void *, void *, s32, s32);

void SceneActorReaction_OnEnemyDefault(void);

void SceneActorReaction_ResolveDamage(void);

void SceneActorReaction_SinkAndRemove(void);

void SceneActorScript_OnCommand(void *actor, const u16 *command);

void AnimData_SelectPaletteBank(void *, s32);

void SceneActorPlacement_HideActor(void *, s32);

void SceneActorBehavior_InstallCallbackTables(void *);

void SceneActorBehavior_InitializeShadow(void *);

void SceneActorTerrain_ReadScriptParameters(void *);

u8 SceneActorRegistry_LinkActor(void *);

extern SceneActorCommandHandler const gUnk_086108B0[];

static inline void MonsterInit_InitializeBody(struct ChaseActor *arg0, u32 *fill)
{
    struct SceneActorBody *body = &arg0->base.body;
    struct ContactPosition origin;
    s32 mode, firstLimit, secondLimit;
    s32 scriptVariant;
    arg0->base.behaviorFlags = 0;
    arg0->base.statusFlags = 0;
    arg0->base.enabled = 0;
    arg0->base.field2E8 = 8;
    arg0->base.placementFlags = MonsterInit_ReadKeywordHalfword(0x4D, 0);
    MonsterInit_ClearPosition(fill, &origin);
    mode = Script_GetKeywordValue(0x4E, 0);

    MonsterInit_ReadTargeting(&MonsterInit_ReadAttacks(arg0)->base.tracking);

    if (Script_SeekToKeyword(0x6C)) {
        firstLimit = MonsterInit_ReadWord(0x12C);
        secondLimit = MonsterInit_ReadWord(0x12C);
    } else {
        firstLimit = 0;
        secondLimit = 0;
    }

    if (Script_GetKeywordValue(0x62, 0))
        MonsterInit_SetStatusFlags(arg0, 0x200);
    arg0->base.script1F0 = MonsterInit_ReadKeywordHalfword(0x46, 0);
    arg0->base.scriptId = MonsterInit_ReadKeywordHalfword(0x52, 0);

    if (Script_SeekToKeyword(0x41)) {
        MonsterInit_SetInitialHealth(arg0, MonsterInit_ReadHalfword(0));
        arg0->base.field9E = MonsterInit_ReadByte(0);
        arg0->base.defense = MonsterInit_ReadByte(0);
        arg0->base.contactReward = MonsterInit_ReadByte(0);
        arg0->base.fieldA2 = MonsterInit_ReadHalfword(0);
        arg0->base.fieldA4 = MonsterInit_ReadHalfword(0);
    }

    MonsterInit_InitializeHealth(arg0, MonsterInit_GetInitialHealth(arg0), firstLimit, secondLimit);

    if (Script_SeekToKeyword(0x58)) {
        arg0->base.experience = MonsterInit_ReadWord(0);
        arg0->base.fieldA1 = MonsterInit_ReadByte(0);
    }
    arg0->base.hudKind = MonsterInit_ReadKeywordHalfword(0x4B, 2);
    scriptVariant = (s16)MonsterInit_ReadKeywordHalfword(0x56, -1);
    if ((s16)scriptVariant < 0)
        scriptVariant = 0xFFFF;
    arg0->base.hudLabel = scriptVariant;
    if (Script_GetKeywordValue(0x48, 0))
        MonsterInit_SetStatusFlags(arg0, 0x800);

    SceneBody_Init(body, mode, &origin, 0, 4, arg0);
    SceneBody_SetSprite(body, MonsterInit_GetDrawableKind(arg0) ? 0 : arg0->base.drawable);
    SceneBody_SetMovementProbe(body, ((struct Slime *)arg0)->base.movementProbe, 0x40, 0x40);
}

static inline void Slime_ReadPalette(struct ChaseActor *actor, u16 base, u16 alternate)
{
    s32 r1v;
    actor->base.paletteOffset = MonsterInit_ReadKeywordHalfword(0x43, 0);
    actor->base.basePaletteBank = base;
    actor->base.paletteAlternateBank = alternate;
    MonsterInit_SetAlternatePaletteOffset(actor, MonsterInit_GetPaletteOffset(actor) + 0xCC);
    r1v = MonsterInit_GetPaletteBase(actor) + MonsterInit_GetPaletteOffset(actor);
    if (MonsterInit_GetDrawableKind(actor) == 0)
        AnimData_SelectPaletteBank(
            &((struct SceneActorAnimationSequenceDrawable *)actor->base.drawable)->graphics, r1v);
    else
        ((struct SceneActorRegistryTileDrawable *)actor->base.drawable)->palette =
            gPaletteBankData + (r1v << 5);
    if (MonsterInit_GetDrawableKind(actor) == 1)
        MonsterInit_SetWidgetPalette(
            &((struct SceneActorAnimationTileDrawable *)actor->base.drawable)->tile,
            SceneActorScript_GetManagerField62() - 0x5FFF);
}

static inline void MonsterInit_InstallPrimary(struct ChaseActor *actor, void *cb0, void *cb1,
                                              void *cb2, void *cb3, void *cb4, void *hidden,
                                              void *cb5, void *cb6, void *defaultReaction,
                                              void *targetQuery, void *damage, void *contact)
{
    MonsterInit_SetBehaviorFlags(actor, 0x2000);
    actor->base.onUpdate = cb0;
    MonsterInit_WriteCallback(&actor->base.animationQuery.opaque, cb1);
    actor->base.onEntry = cb2;
    actor->base.onAlternateEntry = cb3;
    actor->base.onStanding = cb4;
    actor->base.onReaction = cb5;
    actor->base.onDefeat = cb6;
    actor->base.onHidden = hidden;
    actor->base.defaultReaction = defaultReaction;
    MonsterInit_WriteCallback(&actor->base.contactQuery.opaque, 0);
    MonsterInit_WriteCommandTable(&actor->base.commandTable.handlers, gUnk_086108B0);
    actor->base.routeReset = 0;
    actor->base.targetQuery = targetQuery;
    actor->base.sinkCallback = SceneActorReaction_SinkAndRemove;
    actor->base.onDamage = damage;
    actor->base.onContact = contact;
    actor->base.field240 = 0;
    MonsterInit_ClearMotion(actor);
}

static inline void Slime_InstallCompletionCallbacks(struct ChaseActor *actor)
{
    actor->base.onMessage = SceneActorScript_OnCommand;
    actor->base.update = (SceneActorResultCallback)Slime_OnMessage;
    actor->base.destroy = (SceneActorResultCallback)Slime_Destroy;
}

static inline void Slime_InstallRoutes(struct ChaseActor *actor)
{
    actor->base.routeReset = Slime_ResetIdlePlayback;
    actor->base.reactionGate = Slime_FollowPlayer;
    MonsterInit_WriteCallback(&actor->base.contactQuery.opaque, Slime_TryEnrage);
    actor->base.field240 = Slime_PlayPendingSound;
}

static inline u8 Slime_TestFlags(struct Slime *slime, u32 mask)
{
    if (slime->flags & mask)
        return 1;
    return 0;
}

static inline void Slime_SetFlags(struct Slime *slime, u16 mask)
{
    slime->flags |= mask;
}

static inline s32 MonsterInit_UpdateTerrainCell(struct ChaseActor *actor)
{
    struct ContactPosition *position = &actor->base.body.position;
    actor->base.terrainCell = MonsterInit_CellId(position);
    return MonsterInit_TerrainHeight(MonsterInit_GetTerrainCell(actor), position);
}

s32 Slime_Init(void *object)
{
    struct Slime *slime = object;
    struct ChaseActor *actor = object;
    u32 zero;
    struct ContactPosition tmp;
    u32 flag;
    s32 j;
    slime->flags = 0;
    SceneActorRegistry_SetObjectContext(actor, actor, 0, 0);
    MonsterInit_SetCategory(actor, MONSTER_CATEGORY_SLIME);
    Slime_InitProfile(actor);
    if (MonsterInit_GetDrawableKind(actor) == 0) {
        u8 *heap;
        actor->base.drawable = Heap_Alloc(0x5C);
        Memory_Clear(actor->base.drawable, 0x5C);
        heap = MonsterInit_Drawable(actor);
        MonsterInit_InitializeSprite(heap, 0xB96C, 0xA929);
        if (MonsterInit_GetCategory(actor) == 0x11 || MonsterInit_GetCategory(actor) == 0x17)
            heap[7] = 1;
        actor->base.body.direction = 0;
    } else {
        u8 *heap;
        void *res;
        actor->base.drawable = Heap_Alloc(0x80);
        Memory_Clear(actor->base.drawable, 0x80);
        heap = MonsterInit_Drawable(actor);
        MonsterInit_ClearPosition(&zero, &tmp);
        res = ScriptProc_Start(0xCB05, 0xB96C);
        *(struct DisplayResourceHeader *)heap = *(struct DisplayResourceHeader *)res;
        ResourceHeader_Relocate(heap, res);
        Widget_InitLinked((void *)&((struct SceneActorAnimationTileDrawable *)heap)->tile, heap, 0,
                          0, 2, 2, 0x3C, &tmp);
        actor->base.animationIndex = 0xFF;
    }
    MonsterInit_InitializeBody(actor, &zero);
    slime->halfHealth = MonsterInit_GetHealth(actor) >> 1;
    if (Script_SeekToKeyword('p')) {
        s32 ground;
        actor->base.body.position.x = MonsterInit_ReadHalfword(0);
        actor->base.body.position.y = MonsterInit_ReadHalfword(0);
        actor->base.body.position.z = MonsterInit_ReadHalfword(0);
        ground = MonsterInit_UpdateTerrainCell(actor);
        actor->base.groundHeight = ground;
        slime->previousGround = ground;
        if (MonsterInit_GetDrawableKind(actor) == 0)
            ((struct SceneActorAnimationSequenceDrawable *)actor->base.drawable)->sprite.position =
                actor->base.body.position;
        else
            ((struct SceneActorRegistryTileDrawable *)actor->base.drawable)->position =
                actor->base.body.position;
    }
    j = Script_SeekToKeyword('j');
    if (j) {
        actor->base.secondaryMotion.x = MonsterInit_ReadHalfword(0);
        actor->base.secondaryMotion.y = MonsterInit_ReadHalfword(0);
        actor->base.secondaryMotion.z = MonsterInit_ReadHalfword(0);
        Slime_SetFlags(slime, 0x18);
        {
            s32 health = Script_GetKeywordValue('m', 0);
            if (health)
                actor->base.defaultHealth = health;
        }
        actor->base.field2E8 = 1;
    } else {
        actor->base.field2E8 = j;
    }
    actor->base.colliderA.responseValue = MonsterInit_ReadKeywordHalfword('n', 0);
    slime->launchThreshold = 600;
    actor->base.heightOffset = Script_GetKeywordValue('f', 0);
    if (actor->base.heightOffset)
        Slime_SetFlags(slime, 0x10);
    Slime_ReadSplitScript(actor);
    Slime_ReadPalette(actor, 0xCC, 0x192);
    Slime_InitColliders(actor);
    MonsterInit_InstallPrimary(actor, Slime_Update, Slime_GetAnimationMovePulse, 0,
                               Slime_ResetAnimation, Slime_RollMoveFlag, 0,
                               Slime_ConvergeReactionMotion, Slime_RunDefeatScript,
                               SceneActorReaction_OnEnemyDefault, 0,
                               SceneActorReaction_ResolveDamage, Slime_ResolveDamageReaction);
    Slime_InstallRoutes(actor);
    MonsterInit_SetStatusFlags(actor, 8);
    MonsterInit_SetMotionFlags(actor, 0x40);
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
    if (Script_GetKeywordValue(0x53, 0) != 0 || !MonsterInit_IsInCurrentRegion(actor))
        SceneActorPlacement_HideActor(actor, 0);
    SceneActorAnimation_SelectDirectionalEntry(actor, 0);
    if (MonsterInit_GetDrawableKind(actor) == 0)
        DisplaySequence_Step(actor->base.drawable);
    else
        Anim_Step(&((struct SceneActorAnimationTileDrawable *)actor->base.drawable)->tile,
                  actor->base.drawable);
    MonsterInit_ClearReceiver(actor);
    if (Slime_TestFlags(slime, 8)) {
        MonsterInit_SetControl(actor, SceneActorControl_OnIdle, 0x10);
        MonsterInit_SetState(actor, 0, 0, 2);
        MonsterInit_SetStatusFlags(actor, 0x800);
    }
    SceneActorBehavior_InstallCallbackTables(actor);
    Slime_SetPrimaryStateTable(actor);
    Slime_SetSecondaryStateTable(actor);
    MonsterInit_SaveControl(MonsterInit_SavedControlOf(actor), actor);
    Slime_InstallCompletionCallbacks(actor);
    if (!SceneActorRegistry_LinkActor(actor))
        return -1;
    return 0;
}

void Slime_Create(void)
{
    void *m = Heap_Alloc(0x40c);
    if (m != 0) {
        Memory_Clear(m, 0x40c);
        if (Slime_Init(m) < 0) {
            Slime_Destroy(m);
            Heap_Free(m);
        }
    }
}
