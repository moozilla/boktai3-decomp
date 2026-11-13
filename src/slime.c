#include "slime.h"
#include "scene_actor_turn.h"
#include "engine.h"
#include "ram.h"
#include "scene_actor_registry.h"
#include "scene_actor_behavior.h"
#include "scene_actor_control_phase.h"
#include "script_utility.h"
#include "scene_actor_reset.h"
#include "scene_actor_control_motion.h"
#include "scene_actor_animation.h"
#include "display_sequence_inline.h"
#include "scene_actor_chase_reaction.h"
#include "scene_actor_terrain.h"
#include "engine_globals.h"
#include "scene_actor_motion.h"
#include "player.h"
#include "scene_actor_state.h"
#include "scene_body_api.h"
#include "render.h"
#include "scene_actor_init_group.h"
#include "scene_actor_init_graphics.h"

void Slime_ApplyBodyHit(struct ContactCollider *target, struct ContactCollider *info,
                        struct SceneActorRegistryResetView *actor)
{
    s32 k2 = 1;
    if ((!SceneActor_ActionsBlocked((void *)actor)) && (info->contactFlags == k2)) {
        if (!SceneActor_ActionsBlocked((void *)actor) && (info->contactFlags & 0xF)) {
            actor->field304 = info->collisionId;
            {
                u32 *field = &actor->flags2D0;
                u32 mask = ~0x1000000;
                *field &= mask;
            }
        }
        if (actor->paletteOffset == 3) {
            target->reactionFlags = Slime_GetRandomVariantFlags((u8 *)actor);
        }
    }
}

// confidence=low: terrain-edge response is registered as the Slime body collider callback.

void SceneActorBehavior_ApplyContactReaction(struct SceneActorBehaviorContactView *,
                                             struct SceneActorBehaviorContactView *,
                                             struct SceneActorBehaviorView *);

static inline u8 Slime_ContactUnavailable(struct SceneActorRegistryResetView *actor)
{
    if (actor) {
        u32 mask = 0x40;
        if (actor->flags2D4 & mask)
            return 1;
        mask = 0x20000;
        if (actor->flags2D0 & mask)
            return 1;
        if (actor->health <= 0)
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

static inline void Slime_SubtractMotion(struct ContactPosition *out, struct ContactPosition *a,
                                        struct ContactPosition *b)
{
    out->x = a->x - b->x;
    out->y = a->y - b->y;
    out->z = a->z - b->z;
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
                              struct SceneActorRegistryResetView *actor)
{
    struct ContactPosition next, displacement, center, fromContact, adjusted;
    s32 angle;
    u32 contactFlags;
    struct SceneActorRegistryResetView *checkActor = actor;
    if (Slime_ContactUnavailable(checkActor))
        return;
    contactFlags = Slime_ContactFlags(contact, 0x800);

    if (contactFlags) {
        Slime_AddPositions(&next, &actor->body.position, &actor->body.motion);
        Slime_SubtractMotion(&displacement, (void *)&contact->x, &next);
        SceneActorTerrain_GetCenter(actor->terrainCell, &center);
        Slime_SubtractMotion(&fromContact, &center, (void *)&contact->x);
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
        Slime_AddPositions(&adjusted, &actor->body.position, &displacement);
        {
            s32 cell = SceneActorChase_CellAt(&adjusted);
            u32 ground = actor->groundHeight2BE;
            u8 *data = SceneActorTerrain_GetCell(cell);
            if (ground != (*data & 15) << 8)
                SceneActorMotion_ArmPrimaryCollider(actor);
            else {
                actor->body.position.x += displacement.x;
                actor->body.position.z += displacement.z;
                actor->body.motion.x = 0;
                actor->body.motion.z = 0;
            }
        }
    } else
        SceneActorBehavior_ApplyContactReaction((void *)contact, other, (void *)checkActor);
}

void Slime_ResetAnimation(struct SceneActorRegistryResetView *actor)
{
    if (actor->field2B2 != 0)
        actor->field2B2 = 0;
    SceneActorAnimation_SelectProfileEntry((void *)actor, 1);
}

void Slime_StartWalkChoice(struct SceneActorRegistryResetView *actor)
{
    if (Slime_TakePrepared(actor)) {
        SceneActorAnimation_SelectProfileEntry((void *)actor, 1);
        actor->field2C2 = 0x40;
        Anim_SetDurationScale(&((struct SlimeDrawable *)actor->drawable)->sequence, 0x40);
    }
    if (actor->field2B3) {
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

void Slime_RollMoveFlag(struct SceneActorRegistryResetView *actor)
{
    if (Slime_TakePrepared(actor)) {
        u16 *scale = &actor->field2C2;
        *scale = 0x40;
        Anim_SetDurationScale(&((struct SlimeDrawable *)actor->drawable)->sequence, 0x40);
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
            u16 *flags = &actor->flags2D6;
            mask |= *flags;
            *flags = mask;
        } else if (roll <= 13) {
            u32 mask = 0x20;
            u16 *flags = &actor->flags2D6;
            mask |= *flags;
            *flags = mask;
        }
    }
}

void Slime_StartRandomWait(struct SceneActorRegistryResetView *actor)
{
    if (Slime_TakePrepared(actor)) {
        SceneActorAnimation_SelectProfileEntry((void *)actor, 0x11);
        actor->field2C2 = 0x40;
        Anim_SetDurationScale(&((struct SlimeDrawable *)actor->drawable)->sequence, 0x40);
    }
    if (actor->field2B3)
        SceneActorChase_SetControl(actor, Slime_UpdateRandomWait, 0x22);
}

void Slime_UpdateReturnToIdle(struct SceneActorRegistryResetView *actor)
{
    if (Slime_TakePrepared(actor)) {
        SceneActorAnimation_SelectProfileEntry((void *)actor, 0x12);
        actor->field2C2 = 0x40;
        Anim_SetDurationScale(&((struct SlimeDrawable *)actor->drawable)->sequence, 0x40);
    }
    if (actor->field2B3) {
        u8 *q = &actor->state2A8;
        u32 z;
        z = 0;
        *q = z;
        actor->phase2A9 = z;
        actor->behavior2AA = z;
        actor->counter2AC = z;
        actor->field2B1 = 1;
        SceneActorChase_SetControl(actor, SceneActorControl_OnIdle, 0x10);
    }
}

// confidence=low: plays profile animation 15 with a random duration, then enters the next Slime control.

static inline void Slime_SelectWaitAnimation(struct SceneActorAnimationView *actor, s32 index,
                                             u32 variant, u32 inputFlags)
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
        actor->selection = selection;
        drawable = actor->drawable;
        Anim_SetSequence(drawable->sequence, (void *)drawable->resource, selection, variant, flags);
        restart = &actor->restart;
    } else {
        struct SceneActorAnimationTile *tile =
            &((struct SceneActorAnimationTileDrawable *)actor->drawable)->tile;
        u8 *selectionAddress;
        u32 previous;
        u8 *savedSelection;
        selection += variant;
        selectionAddress = &actor->selection;
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
            u32 mask = 4;
            u32 mode;
            if (flags & mask)
                mode = 6;
            else
                mode = 4;
            Anim_SetKeyframes(&((struct SceneActorAnimationTileDrawable *)actor->drawable)->tile,
                              actor->drawable, (u16)selection, 2, mode);
            {
                u8 playbackFlags = flags;
                if (playbackFlags & 1)
                    tile->flags |= mask;
                else
                    tile->flags &= ~4;
            }
        }
    }
    *restart = 0;
}

void Slime_UpdateRandomWait(struct SceneActorRegistryResetView *actor)
{
    if (Slime_TakePrepared(actor)) {
        u32 index;
        Slime_SelectWaitAnimation((void *)actor, 15, 0, 0);
        index = (gRandomIndex + 1) & 1023;
        gRandomIndex = index;
        {
            u32 random = *(u16 *)(index * 2 + (u32)gRandomTable);
            actor->field2C2 = random & 63;
        }
        if (!actor->field2C2)
            actor->field2C2 = 64;
        Anim_SetDurationScale(&((struct SlimeDrawable *)actor->drawable)->sequence,
                              actor->field2C2);
    }
    if (actor->field2B3)
        SceneActorChase_SetControl(actor, Slime_PlayWaitAnimation, 0x23);
}

void Slime_PlayWaitAnimation(struct SceneActorRegistryResetView *actor)
{
    if (Slime_TakePrepared(actor))
        SceneActorTurn_SelectProfileAnimation((void *)actor, 15, 1, 0);
    if (actor->field2B3)
        SceneActorChase_SetControl(actor, Slime_TickWaitTimer, 0x24);
}

void Slime_TickWaitTimer(struct SceneActorRegistryResetView *actor)
{
    struct Slime *q = actor->context;
    if (Slime_TakePrepared(actor)) {
        u32 index = (gRandomIndex + 1) & 1023;
        u32 random;
        gRandomIndex = index;
        random = *(u16 *)(index * 2 + (u32)gRandomTable);
        q->waitTimer = (random & 63) + 0x40;
    }
    if (q->waitTimer)
        q->waitTimer--;
    else if (actor->field2B3)
        SceneActorChase_SetControl(actor, Slime_UpdateRandomWait, 0x22);
}

void Anim_SetDurationScale(void *, u32);

static inline u8 Slime_TakeEntryFlag(struct SceneActorRegistryResetView *p)
{
    if (p->field2B2) {
        p->field2B2 = 0;
        return 1;
    }
    return 0;
}

static inline void Slime_ClearControlFlags(u32 *flags, u32 mask)
{
    u32 m = ~mask;
    *flags &= m;
}

static inline void Slime_SetNextControl(struct SceneActorRegistryResetView *p, void (*fn)(void *),
                                        u32 wait)
{
    u8 *flag = &p->field2B2;
    u32 zero = 0;
    *flag = 1;
    p->field2B0 = zero;
    p->field2B5 = wait;
    p->target = fn;
    p->field2C8 = zero;
    Slime_ClearControlFlags(&p->flags114, 1);
    p->word104 = zero;
}

void Slime_UpdateWalkTransition(struct SceneActorRegistryResetView *p)
{
    if (Slime_TakeEntryFlag(p)) {
        SceneActorAnimation_SelectProfileEntry((void *)p, 0xe);
        Anim_SetDurationScale(&((struct SlimeDrawable *)p->drawable)->sequence, 0x40);
    }
    if (p->field2B3 != 0) {
        u32 index = (gRandomIndex + 1) & 1023;
        s32 r;
        gRandomIndex = index;
        r = *(u16 *)(index * 2 + (u32)gRandomTable);
        r &= 0x1f;
        if (r <= 0x17)
            Slime_SetNextControl(p, Slime_RollMoveFlag, 0);
        else
            Slime_SetNextControl(p, Slime_StartWalkChoice, 0x29);
    }
}

void Slime_OnUnusedControlEvent(void)
{
}

// confidence=low: motion converges to saved displacement before choosing idle or the next Slime control.

static inline s32 Slime_QuarterMotion(s32 v)
{
    return v >= 0 ? v >> 2 : -(-v >> 2);
}

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

static inline u32 Slime_IsReactionComplete(struct SceneActorRegistryResetView *actor)
{
    u32 complete = actor->field2B3;
    return complete;
}

void Slime_ConvergeReactionMotion(struct SceneActorRegistryResetView *actor)
{
    struct Slime *context = actor->context;
    struct DisplaySequenceState *sequence =
        (void *)(&((struct SlimeDrawable *)actor->drawable)->sequence);
    s32 x;
    s32 z;
    s16 *priorX;
    s16 *priorZ;
    u32 *actorFlags;
    u32 *t;
    if (Slime_TakePrepared(actor)) {
        actor->secondaryMotion.x = Slime_QuarterMotion(actor->secondaryMotion.x);
        actor->secondaryMotion.z = Slime_QuarterMotion(actor->secondaryMotion.z);
        SceneActorAnimation_SelectProfileEntry((void *)actor, 3);
        {
            u16 *flags = &actor->flags2D4;
            u32 mask = ~1;
            *flags = mask & *flags;
        }
        actor->field2B3 = 0;
    }
    {
        u32 mask = 0x2000;
        t = &actor->flags2CC;
        *t |= mask;
    }
    {
        u32 mask = 0x10000;
        actor->flags2D0 |= mask;
    }
    {
        s32 current = actor->reactionStart.x;
        s16 *field = &actor->reactionEnd.x;
        s32 difference = current - *field;
        actorFlags = t;
        priorX = field;
        x = Slime_QuarterMotion(difference);
    }
    {
        s32 current = actor->reactionStart.z;
        s16 *field = &actor->reactionEnd.z;
        s32 difference = current - *field;
        priorZ = field;
        z = Slime_QuarterMotion(difference);
    }
    x = Slime_ClampMotion(x);
    z = Slime_ClampMotion(z);
    actor->body.motion.x += x;
    actor->body.motion.z += z;
    *priorX += x;
    *priorZ += z;
    {
        u32 rest = Slime_MotionAtRest(x, z);
        if (!rest)
            sequence->elapsed = rest;
        else if (Slime_IsReactionComplete(actor)) {
            u8 *field = &actor->field2B0;
            u32 ready = 1;

            *field = ready;
            {
                u16 *flags = &actor->flags2D4;
                u32 mask = ready - 34;
                *flags = mask & *flags;
            }
            {
                u32 mask = ~0x80;
                *actorFlags = mask & *actorFlags;
            }
            if (actor->health < context->halfHealth || Slime_HasContextFlag(context, 8)) {
                SceneActorChase_SetControl(actor, SceneActorControl_OnIdle, 0x10);
                SceneActorChase_SetBehavior(actor, 2);
                {
                    u16 *flags = &actor->flags2D6;
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

void Slime_UpdateShake(struct SceneActorRegistryResetView *actor, s32 elapsed)
{
    struct Slime *q = actor->context;
    struct DisplaySequenceState *st =
        (struct DisplaySequenceState *)(&((struct SlimeDrawable *)actor->drawable)->sequence);
    if (Slime_TakePrepared(actor)) {
        q->enrageCount = 0;
        actor->field2C2 = 0x40;
        Anim_SetDurationScale(&((struct SlimeDrawable *)actor->drawable)->sequence, 0x40);
        SceneActorAnimation_SelectProfileEntry(actor, 1);
        {
            u32 bit = 0x100;
            actor->flags2D4 = bit | actor->flags2D4;
        }
    }
    st->index = 0;
    st->elapsed = 5;
    if (Mod(elapsed, 4) == 0) {
        u32 mask = 0x20;
        u16 *flags = &actor->flags2D6;
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

void Slime_UpdateLaunch(struct SceneActorRegistryResetView *actor)
{
    struct SlimeDrawable *d = actor->drawable;
    u32 mask;
    struct DisplaySequenceState *anim = (struct DisplaySequenceState *)&d->sequence;
    if (Slime_TakePrepared(actor)) {
        struct ContactPosition *v;
        u32 heading;
        s32 speed;
        u8 *hp;
        actor->field2C2 = 0x20;
        Anim_SetDurationScale(&((struct SlimeDrawable *)actor->drawable)->sequence, 0x40);
        hp = &actor->tracking.heading;
        v = &actor->secondaryMotion;
        heading = *hp;
        speed = 0x12;
        v->x = Shift12(gUnk_086149C4[(heading + 0x40) & 0xFF] * speed);
        v->y = 0;
        v->z = Shift12(gUnk_086149C4[heading] * speed);
        actor->secondaryMotion.y = 0x28;
        d->flags |= 2;
        d->scaleX = 0x40;
        d->scaleY = 0x40;
        Sound_PlaySong(0x3C1);
        actor->field2E8 = 0;
    }
    if (anim->index == 3)
        anim->elapsed = 0;
    if (actor->secondaryMotion.y > 0) {
        if (d->scaleY <= 0x7E)
            d->scaleY = d->scaleY + 1;
    } else {
        if (d->scaleY > 0x40)
            d->scaleY = d->scaleY - 2;
        if (d->scaleY <= 0x3F)
            d->scaleY = 0x40;
    }
    mask = 0x10;
    if (actor->flags2D0 & mask) {
        SceneActorChase_SetControl(actor, Slime_UpdateStill, 0x2C);
        Slime_EmitScatteredReactionBurst(actor);
        Slime_EmitScatteredReactionBurst(actor);
        Sound_PlaySong(0x3C2);
    }
}

void Slime_UpdateStill(struct SceneActorRegistryResetView *actor)
{
    struct SlimeDrawable *d = actor->drawable;
    if (Slime_TakePrepared(actor)) {
        d->scaleX = 0x40;
        d->scaleY = 0x40;
        d->flags &= ~2;
        {
            u16 *f = &actor->flags2D4;
            u32 mask = 0xFFFFFEFF;
            *f = mask & *f;
        }
    }
    if (actor->field2B3) {
        actor->field2C2 = 0x40;
        SceneActorChase_SetControl(actor, SceneActorControl_OnIdle, 0x10);
        SceneActorChase_SetState(actor, 0, 0, 2);
    }
}

/* SceneActorControl_CompleteScriptedResponse_0809F368: SceneActorControl_CompleteScriptedResponse with other constants (ported by relatives.py, fixed by hand; provisional name) */

void *BestiaryDisplay_GetEntryForActor(u32, u32);

void GameState_SetEventFlag(void *);

void SceneActorBehavior_CallTerrainScript(struct SceneActorBehaviorView *);

void SceneActorPlacement_PlayCategoryEventSound(struct SceneActorRegistryActor *);

u8 Sprite_BlinkUntilExpired(void *, s32, s32, s32);

static inline void Slime_SetReactionState(struct SceneActorRegistryResetView *actor, u8 state,
                                          u8 phase, u8 behavior)
{
    actor->state2A8 = state;
    actor->phase2A9 = phase;
    actor->behavior2AA = behavior;
    actor->counter2AC = 0;
    actor->field2B1 = 1;
}

/* Explicit branches retain the original shared true/false exit blocks. */

static inline u8 Slime_IsHidden(struct SceneActorRegistryResetView *a)
{
    u32 mask = 1;
    if (!a->drawableKind) {
        if (((struct SlimeDrawable *)a->drawable)->flags & mask)
            goto yes;
        else
            goto no;
    } else {
        if (((struct SceneActorRegistryTileDrawable *)a->drawable)->flags & mask)
            goto yes;
        else
            goto no;
    }
yes:
    return 1;
no:
    return 0;
}

void Bat_RunDefeatScript(struct SceneActorRegistryResetView *actor, s32 frame)
{
    s32 values[8];
    struct ScriptUtilityArguments arguments;
    u16 script;
    u32 *stateFlags;
    if (SceneActorControl_TakeFlag(&actor->field2B2)) {
        SceneActorAnimation_SelectProfileEntry((void *)actor, 10);
        SceneActorPlacement_PlayCategoryEventSound((void *)actor);
        Slime_EmitDeathDebris(actor);
        {
            u16 *p = &actor->flags2D4;
            u32 mask = ~0x38;
            *p = mask & *p;
        }
    }
    {
        u32 mask = 0x8000;
        u32 *p = &actor->flags2CC;
        u32 v = *p & mask;
        stateFlags = p;
        if (v)
            return;
    }

    if (Sprite_BlinkUntilExpired(actor->drawable, frame, 10, 30)) {
        actor->field2B0 = 1;
        script = (u16)((struct SceneActorBehaviorView *)actor)->script1F8;
        if (script) {
            values[0] = actor->body.position.x;
            values[1] = actor->body.position.y;
            values[2] = actor->body.position.z;
            values[3] = ((struct SceneActorBehaviorView *)actor)->parameters1FC[0];
            values[4] = ((struct SceneActorBehaviorView *)actor)->parameters1FC[1];
            values[5] = ((struct SceneActorBehaviorView *)actor)->parameters1FC[2];
            values[6] = actor->field2E0;
            values[7] = actor->body.id;
            arguments.count = 8;
            arguments.values = values;
            Script_Call(script, &arguments);
        }
        {
            u32 mask = 0x4000;
            if (!(actor->flags2D4 & mask) && (u32)(actor->category - 11) > 2)
                GameState_SetEventFlag(
                    BestiaryDisplay_GetEntryForActor(actor->category, actor->paletteOffset));
        }
        {
            u32 mask = 0x400;
            if (!(*stateFlags & mask))
                SceneActorBehavior_CallTerrainScript((void *)actor);
        }
        {
            void *callback = (void *)SceneActorControl_OnIdle;
            u32 kind;
            kind = 16;
            actor->field2B2 = 1;
            actor->field2B0 = 0;
            actor->field2B5 = kind;
            actor->target = callback;
            actor->field2C8 = 0;
            {
                u32 *flags = &actor->flags114;
                u32 mask = ~1;
                *flags &= mask;
            }
            actor->word104 = 0;
        }
        Slime_SetReactionState(actor, 1, 0, 2);
    } else {
        if (Slime_IsHidden(actor))
            ActorShadow_Disable(actor->shadow);
        else
            ActorShadow_Enable(actor->shadow);
    }
}

void SceneActorBehavior_CallActionScript(u8 *);

inline static u8 Slime_TakeActionEntry(u8 *p)
{
    if (*p) {
        *p = 0;
        return 1;
    }
    return 0;
}

void Bat_UpdateActionScript(struct SceneActorRegistryResetView *obj)
{
    struct Slime *context = obj->context;

    if (Slime_TakeActionEntry(&obj->field2B1)) {
        obj->body.direction = obj->tracking.heading;
        SceneActorBehavior_CallActionScript((u8 *)obj);
    }
    if (obj->field2B0) {
        u32 flag = 4;
        u16 *h = &context->flags;
        flag |= *h;
        *h = flag;
    } else {
        ++obj->counter2AC;
    }
}

extern void *gUnk_08605D14[];

void Slime_SetSecondaryStateTable(struct SceneActorRegistryResetView *actor)
{
    actor->alternateBehaviorCallbacks = gUnk_08605D14;
}

s32 Math_Atan2(s32, s32);

s32 Callback_InvokeActor(void *, void *);

void Anim_SetFrameDurationScale(void *, u32);

static inline u8 Slime_TakeApproachEntry(u8 *p)
{
    if (*p) {
        *p = 0;
        return 1;
    }
    return 0;
}

static inline s32 Slime_ShiftTowardZero(s32 n, u32 bits)
{
    if (n >= 0)
        return n >> bits;
    return -((-n) >> bits);
}

static inline void Slime_SetApproachControl(struct SceneActorRegistryResetView *a, void *cb,
                                            u32 kind)
{
    u32 zero;
    a->field2B2 = 1;
    zero = 0;
    a->field2B0 = zero;
    a->field2B5 = kind;
    a->target = cb;
    a->field2C8 = zero;
    {
        u32 *flags = &a->flags114;
        u32 mask = ~1;
        *flags &= mask;
    }
    a->word104 = zero;
}

static inline u8 Slime_ApproachWanderTarget(struct SceneActorRegistryResetView *actor,
                                            struct SceneActorMotionSubject *target, s32 speed)
{
    struct SceneActorBody *motion = &actor->body;
    s16 dx = target->position.x - motion->position.x;
    s16 dz = target->position.z - motion->position.z;
    if ((dx < 0 ? -dx : dx) <= 32 && (dz < 0 ? -dz : dz) <= 32)
        return 1;
    if (dx || dz) {
        motion->direction = Math_Atan2(dx, dz);
        motion->motion.x +=
            Slime_ShiftTowardZero(gUnk_086149C4[(motion->direction + 64) & 255] * speed, 12);
        motion->motion.z += Slime_ShiftTowardZero(gUnk_086149C4[motion->direction] * speed, 12);
    }
    return 0;
}

static inline void Slime_SetMotionScale(struct SceneActorRegistryResetView *actor)
{
    u16 *scale;
    {
        u32 mask = 0xA02F;
        if (actor->flags2D4 & mask) {
            u16 *p = &actor->field2C2;
            *p = 64;
            scale = p;
        } else {
            u16 *q = &actor->field2C2;
            u16 v = *q;
            scale = q;
            if (v == 0)
                *scale = 64;
        }
    }
    if (actor->drawableKind == 0)
        Anim_SetDurationScale(&((struct SlimeDrawable *)actor->drawable)->sequence, *scale);
    else
        Anim_SetFrameDurationScale(
            &((struct SceneActorAnimationTileDrawable *)actor->drawable)->tile, *scale);
}

// confidence=low: moves toward a saved waypoint, then re-selects the route.

void Slime_ApproachSavedTarget(struct SceneActorRegistryResetView *actor)
{
    struct Slime *context = actor->context;
    s32 speed;
    if (Slime_TakeApproachEntry(&actor->field2B1)) {
        Slime_SetApproachControl(actor, actor->entryTarget228, 1);
        Slime_SelectWanderTarget(actor, actor->body.direction);
        actor->field2C2 = 0x40;
        return;
    }
    Callback_InvokeActor(actor, actor->contactQuery.opaque);
    speed = Callback_InvokeActor(actor, actor->animationQuery.opaque);
    Slime_SetMotionScale(actor);
    if (Slime_ApproachWanderTarget(actor, &context->wanderTarget, speed))
        Slime_SelectWanderTarget(actor, actor->body.direction);
    {
        u32 m = 0x2000;
        u32 f = actor->flags2CC;
        f |= m;
        actor->flags2CC = f;
    }
    actor->counter2AC++;
}

void Slime_RestoreSavedTarget(struct SceneActorRegistryResetView *actor)
{
    if (SceneActorControl_TakeStateEntry(actor))
        SceneActorChase_SetControl(actor, actor->savedTarget, 0);
    Callback_InvokeActor(actor, actor->contactQuery.opaque);
    actor->counter2AC++;
}

void Slime_RunUpdateThenWait(struct SceneActorRegistryResetView *actor)
{
    u8 result;
    if (actor->field2B1)
        actor->field2B1 = 0;
    result = Callback_InvokeActor(actor, actor->contactQuery.opaque);
    if (!result) {
        SceneActorChase_SetControl(actor, Slime_UpdateReturnToIdle, 0x27);
        SceneActorChase_SetState(actor, 3, 1, 0);
    }
    actor->counter2AC++;
}

extern void *gUnk_08605D18[];

void Slime_SetPrimaryStateTable(struct SceneActorRegistryResetView *actor)
{
    actor->callbacks284 = gUnk_08605D18;
}

void SceneActorCollision_InitBody(u8 *, struct SlimeCollisionVector *,
                                  struct SlimeCollisionVector *, void *, u8 *);

void SceneActorCollision_InitAttack(u8 *, struct SlimeCollisionVector *,
                                    struct SlimeCollisionVector *, void *, u32, u8 *);

void Slime_InitColliders(struct SceneActorRegistryResetView *actor)
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

void Slime_ResetIdlePlayback(struct SceneActorRegistryResetView *actor)
{
    struct Slime *context = actor->context;
    context->flags = 0;
    context->enrageCount = 0;
    SceneActorAnimation_SelectDirectionalEntry(actor, 0);
    {
        struct SceneActorControlMotionDrawable *drawable = actor->drawable;
        DisplaySequence_StepState(drawable, &drawable->sequence);
    }
}

/* Slime_OnUnusedEvent: Empty function in the Slime implementation; event role is tentative */

void Slime_OnUnusedEvent(void)
{
}

void Slime_InitProfile(struct SceneActorRegistryResetView *actor)
{
    actor->profileId = SceneActorProfiles_FindOrAdd(0x16, 0);
}

s32 Mod(s32, s32);

extern const u16 gUnk_08605D58[];

// confidence=low: emits the profile's six-frame movement pulse at animation frame boundaries.

s32 Slime_GetAnimationMovePulse(struct SceneActorRegistryResetView *actor)
{
    struct DisplaySequenceState *sequence =
        &((struct SceneActorControlMotionDrawable *)actor->drawable)->sequence;
    s32 result;
    if (sequence->elapsed)
        result = 0;
    else {
        u32 mask = 0x10000;
        actor->flags2D0 |= mask;
        result = Mod(sequence->index, 6);
        {
            const u16 *table = gUnk_08605D58;
            result = table[result];
        }
    }
    return result;
}

u8 SceneActorTerrain_CanTraverseCell(u32, u32, u32, u32);

static inline s32 Slime_QuarterDirection(s32 heading)
{
    if (heading >= 0)
        return heading >> 6;
    return -(-heading >> 6);
}

static inline u16 Slime_RandomDirection(void)
{
    u32 index = (gRandomIndex + 1) & 1023;
    gRandomIndex = index;
    index *= 2;
    return *(const u16 *)(RANDOM_TABLE_ADDR + index);
}

static inline s32 Slime_MapWidth(struct EngineMap **terrain)
{
    return ((struct SceneActorChaseTerrainGrid *)(*terrain)->cells)->width;
}

static inline s32 Slime_ForwardCell(struct SceneActorRegistryResetView *actor, s32 steps)
{
    s32 cell = actor->terrainCell;
    struct ContactPosition *position = &actor->body.position;
    struct EngineMap **cachedMapSlot = &gTerrainMap;
    u32 height = SceneActorChase_HeightAt(cell, position);
    u32 shape = SceneActorChase_CellProperties(cell)->attributes >> 4;
    s32 direction = Slime_QuarterDirection(actor->body.direction + 32) & 3;
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
            next = cell + Slime_MapWidth(cachedMapSlot);
            i = 0;
            completionMapSlot = &gTerrainMap;
            if (i < steps)
                do {
                    if (!SceneActorTerrain_CanTraverseCell(cell, next, height, shape))
                        goto south;
                    next += Slime_MapWidth(cachedMapSlot);
                } while (++i < steps);
            return next - Slime_MapWidth(completionMapSlot);
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
            next = cell - Slime_MapWidth(cachedMapSlot);
            i = 0;
            completionMapSlot = &gTerrainMap;
            if (i < steps)
                do {
                    if (!SceneActorTerrain_CanTraverseCell(cell, next, height, shape))
                        goto north;
                    next -= Slime_MapWidth(cachedMapSlot);
                } while (++i < steps);
            return next + Slime_MapWidth(completionMapSlot);
        default:
            goto home;
    }
south:
    return next - Slime_MapWidth(cachedMapSlot);
north:
    return next + Slime_MapWidth(cachedMapSlot);
home:
    return actor->terrainCell;
}

void Slime_SelectWanderTarget(struct SceneActorRegistryResetView *actor, u32 direction)
{
    struct ContactPosition center;
    struct Slime *context = actor->context;
    s32 tries, chosen;
    actor->body.direction = Slime_RandomDirection();
    /* The selected exit preserves the original loop rotation in agbcc. */
    tries = 0;
    do {
        chosen = Slime_ForwardCell(actor, 2);
        if (chosen != actor->terrainCell)
            goto selected;
        actor->body.direction += 64;
        tries++;
    } while (tries <= 3);
selected:
    actor->body.direction = direction;
    SceneActorTerrain_GetCenter(chosen, &center);
    context->wanderTarget.position = center;
}

static inline u8 Slime_TestFlag(struct SceneActorRegistryResetView *actor)
{
    struct SceneActorChaseTracking *p = &actor->tracking;
    u32 mask = 2;
    if (p->flags & mask)
        return 1;
    return 0;
}

static inline u8 Slime_TestContext(struct Slime *q)
{
    u32 mask = 8;
    if (q->flags & mask)
        return 1;
    return 0;
}

static inline void Slime_SetFlags2D6(struct SceneActorRegistryResetView *actor, u32 bits)
{
    u32 mask = bits;
    u16 *flags = &actor->flags2D6;
    mask |= *flags;
    *flags = mask;
}

u32 Slime_TryEnrage(struct SceneActorRegistryResetView *actor)
{
    struct Slime *q = actor->context;
    s32 dy;
    actor->field2C2 = 0x40;
    if (!Slime_TestFlag(actor))
        return 0;
    dy = actor->body.position.y -
         ((struct SceneActorChasePlayer *)gUnknown_02000580[0])->target.position.y;
    if (dy < 0)
        dy = -dy;
    if (dy > 0x200)
        return 0;
    if (actor->tracking.distance > actor->tracking.limit)
        return 0;
    if (actor->health < q->halfHealth || Slime_TestContext(q)) {
        Slime_SetFlags2D6(actor, 0x80);
        {
            struct SceneActorChaseTracking *p = &actor->tracking;
            u32 mask = 0x10;
            mask |= p->flags;
            p->flags = mask;
        }
        actor->field2C2 = 0x20;
        q->enrageCount++;
    } else {
        Slime_SetFlags2D6(actor, 0x100);
    }
    return 1;
}

void Slime_FollowPlayer(struct SceneActorRegistryResetView *actor)
{
    struct Slime *q = actor->context;
    struct SceneActorChasePlayer *p;
    u32 zero;
    struct ContactPosition *dst;
    struct SceneActorReactionMotion *m = (struct SceneActorReactionMotion *)&actor->tracking.target;
    {
        u32 v = 8;
        u32 f = m->flags;
        zero = 0;
        v |= f;
        m->flags = v;
    }
    dst = &q->wanderTarget.position;
    p = (struct SceneActorChasePlayer *)gUnknown_02000580[0];
    *dst = p->target.position;
    if (q->enrageCount >= q->launchThreshold) {
        SceneActorChase_SetControl(actor, Slime_UpdateShake, 0x2A);
        SceneActorChase_SetState(actor, 3, zero, zero);
    }
}

void Slime_OnInterrupt(struct SceneActorRegistryResetView *actor)
{
    u8 kind = actor->field2B5;
    if (kind != 0x26 && kind != 0x22 && kind != 0x23 && kind != 0x24) {
        SceneActorChase_SetControl(actor, Slime_StartRandomWait, 0x26);
        SceneActorChase_SetState(actor, 0, 0, 0xF);
    }
}

void Bat_ReadScriptValuePair(struct SceneActorRegistryResetView *actor)
{
    struct Slime *context = actor->context;
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

void Slime_RunPendingActionScript(struct SceneActorRegistryResetView *actor)
{
    s32 values[10];
    struct ContactPosition vector;
    struct ScriptUtilityArguments args;
    struct Slime *context = actor->context;
    if (context->splitScript) {
        values[0] = actor->body.position.x;
        values[1] = actor->body.position.y;
        values[2] = actor->body.position.z;
        values[3] = actor->health;
        Slime_ScriptPolarMotion(&vector, actor->field2B6, 12);
        values[4] = vector.x;
        values[5] = 24;
        values[6] = vector.z;
        values[7] = actor->paletteOffset;
        values[8] = context->splitParameter;
        values[9] = (u16)actor->defaultHealth;
        args.count = 10;
        args.values = values;
        Script_Call(context->splitScript, &args);
        Slime_EmitDeathDebris(actor);
        context->splitScript = 0;
        {
            u32 mask = 8;
            u16 *p = &context->flags;
            *p = (*(&mask)) | (*p);
        }
        {
            void *callback = (void *)SceneActorControl_OnIdle;
            u32 kind = 16;
            actor->field2B2 = 1;
            actor->field2B0 = 0;
            actor->field2B5 = kind;
            actor->target = callback;
            actor->field2C8 = 0;
            {
                u32 *p = &actor->flags114;
                u32 mask = ~1;
                *p &= mask;
            }
            actor->word104 = 0;
        }
        Slime_SetReactionState(actor, 0, 0, 2);
        {
            u16 *p = &actor->flags2D6;
            u32 mask = ~0x980;
            *p = mask & *p;
        }
    }
}

u32 Slime_GetRandomVariantFlags(struct SceneActorRegistryResetView *actor)
{
    s32 v;
    u32 r;
    if (actor->paletteOffset != 3)
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

static inline void Slime_DebrisPalette(struct SceneActorRegistryResetView *actor, u16 **bank,
                                       u16 **offset)
{
    *bank = &actor->basePaletteBank;
    *offset = &actor->paletteOffset;
}

void Slime_EmitDeathDebris(struct SceneActorRegistryResetView *actor)
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
        base = &((struct SceneActorRegistryResetView *)base)->body.position;
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

static inline void Slime_TargetBurstDelta(struct ContactPosition *v, struct ContactPosition *from,
                                          struct ContactPosition *to)
{
    v->x = to->x - from->x;
    v->y = to->y - from->y;
    v->z = to->z - from->z;
}

static inline u16 *Slime_TargetBurstPalette(struct SceneActorRegistryResetView *actor)
{
    return &actor->basePaletteBank;
}

static inline u16 *Slime_TargetBurstOffset(struct SceneActorRegistryResetView *actor)
{
    return &actor->paletteOffset;
}

static inline u16 *Slime_TargetBurstPower(struct SceneActorRegistryResetView *actor)
{
    return &actor->attacks.first[1];
}

static inline u16 *Slime_TargetBurstParameter(struct SceneActorRegistryResetView *actor)
{
    return &actor->attacks.second[1];
}

static inline u32 *Slime_TargetBurstFlags(struct SceneActorRegistryResetView *actor)
{
    return &actor->attacks.flags[1];
}

// confidence=low: launches three randomized reaction projectiles toward the current target.

void Slime_EmitTargetedReactionBurst(struct SceneActorRegistryResetView *input)
{
    struct SceneActorRegistryResetView *actor = input;
    struct ContactPosition velocity;
    s32 angle;
    u32 variant = Slime_GetRandomVariantFlags(actor);
    s16 jitter = Slime_TargetBurstRandom() & 15;
    Slime_TargetBurstDelta(&velocity, &actor->body.position, &actor->tracking.target->position);
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
        value = &actor->attacks.parameter[1];
        v = *value;
        position = &actor->body.position;
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
        value = &actor->attacks.parameter[1];
        v = *value;
        position = &actor->body.position;
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
        value = &actor->attacks.parameter[1];
        v = *value;
        position = &actor->body.position;
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

static inline void Slime_SubtractPositions(struct ContactPosition *v, struct ContactPosition *from,
                                           struct ContactPosition *to)
{
    v->x = to->x - from->x;
    v->y = to->y - from->y;
    v->z = to->z - from->z;
}

static inline u16 *Slime_PaletteBase(struct SceneActorRegistryResetView *actor)
{
    return &actor->basePaletteBank;
}

static inline u16 *Slime_PaletteOffset(struct SceneActorRegistryResetView *actor)
{
    return &actor->paletteOffset;
}

static inline u16 *Slime_AttackPower(struct SceneActorRegistryResetView *actor)
{
    return &actor->attacks.first[1];
}

static inline u16 *Slime_AttackResponse(struct SceneActorRegistryResetView *actor)
{
    return &actor->attacks.second[1];
}

static inline u32 *Slime_AttackFlags(struct SceneActorRegistryResetView *actor)
{
    return &actor->attacks.flags[1];
}

static inline u16 *Slime_AttackParameter(struct SceneActorRegistryResetView *actor)
{
    return &actor->attacks.parameter[1];
}

static inline struct ContactPosition *Slime_Position(struct SceneActorRegistryResetView *actor)
{
    return &actor->body.position;
}

/* Emit three reaction projectiles with independent random headings. */

void Slime_EmitScatteredReactionBurst(struct SceneActorRegistryResetView *input)
{
    struct SceneActorRegistryResetView *actor = input;
    struct ContactPosition velocity;
    s32 angle;
    u32 variant = Slime_GetRandomVariantFlags(actor);
    s32 jitter = Slime_RandomSample() & 15;
    Slime_SubtractPositions(&velocity, &actor->body.position, &actor->tracking.target->position);
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

static inline u8 Slime_TestPendingFlags(u16 *m, u32 b)
{
    if (*m & b)
        return TRUE;
    return FALSE;
}

void Slime_PlayPendingSound(struct SceneActorRegistryResetView *actor)
{
    struct Slime *context = actor->context;
    u32 m = 0x10;
    if (Slime_TestPendingFlags(&context->flags, m)) {
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
    {
        u32 mask = 0x2000;
        u32 *flags;
        flags = &actor->flags2CC;
        *flags |= mask;
    }
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

static inline void Slime_SetReactionControl(struct SceneActorRegistryResetView *actor, void *cb,
                                            u32 k)
{
    void *callback = cb;
    u32 kind = k;
    actor->field2B2 = 1;
    actor->field2B0 = 0;
    actor->field2B5 = kind;
    actor->target = callback;
    actor->field2C8 = 0;
    {
        u32 *field = &actor->flags114;
        u32 mask = ~1;
        *field &= mask;
    }
    actor->word104 = 0;
}

static inline void Slime_SetDamageState(struct SceneActorRegistryResetView *actor, u32 s, u32 p,
                                        u32 b)
{
    u32 behavior = b;
    actor->state2A8 = s;
    actor->phase2A9 = p;
    actor->behavior2AA = behavior;
    actor->counter2AC = 0;
    actor->field2B1 = 1;
}

static inline u32 Slime_CollisionDamageFlags(struct ContactCollider *p)
{
    return p->statRewardFlags;
}

static inline u32 Slime_MaskedDamage(struct SceneActorRegistryResetView *actor, u32 flags, u32 mask)
{
    return actor->field2DC & flags & mask;
}

static inline u32 Slime_MaskedMotionBits(u16 *p, u32 mask)
{
    return *p & mask;
}

static inline u32 Slime_DamageFlags(u32 *flags)
{
    return *flags;
}

// confidence=low: reaction callback chooses split/idle/recovery states after damage.

u32 Slime_ResolveDamageReaction(struct SceneActorRegistryResetView *actor)
{
    struct Slime *context = actor->context;
    u16 *motionFlags;
    u32 zero;
    u32 damage;
    if (actor->reactionSoundVariant == 0)
        Sound_PlaySong(0x211);
    else
        Sound_PlaySong(0x3bf);
    if (actor->paletteOffset == 3)
        Slime_EmitTargetedReactionBurst(actor);
    Slime_ClearMotionBits(&actor->flags2D4, ~0x20);
    context->enrageCount += 80;
    damage = Slime_MaskedDamage(actor, Slime_CollisionDamageFlags(&actor->colliderA), 0x7f);
    motionFlags = &actor->flags2D4;
    if (damage == 0 && context->splitScript) {
        if (actor->health < context->halfHealth)
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
    if ((Slime_DamageFlags(&actor->field2DC) & 0x40000)) {
        Slime_ClearReactionBits(&actor->flags2D6, ~0x980);
        Slime_SetReactionControl(actor, actor->reactionTarget230, 0x12);
        Slime_SetDamageState(actor, 3, 0, 0);
    } else {
        if (Slime_MaskedMotionBits(motionFlags, 0x100))
            return 0;
        if (actor->health < context->halfHealth || (zero = Slime_TestReactionFlag(context, 8))) {
            Slime_SetReactionControl(actor, SceneActorControl_OnIdle, 0x10);
            Slime_SetDamageState(actor, 0, 0, 2);
            Slime_ClearReactionBits(&actor->flags2D6, ~0x980);
        } else if ((*(u32 *)&actor->state2A8 & 0xffffff) != 0) {
            Slime_SetReactionControl(actor, actor->savedTarget, 0);
            Slime_SetDamageState(actor, 0, 0, 0);
        }
    }
    return 1;
}

u32 Slime_ProcessPendingStateChange(struct SceneActorRegistryResetView *actor)
{
    u32 m = 0x100;
    if (Slime_TestPendingFlags(&actor->flags2D6, m)) {
        Slime_OnInterrupt(actor);
        Slime_ClearPendingFlags(&actor->flags2D6, ~0x100);
    }
    return 1;
}

s32 SceneActorMotion_RefreshGroundHeight(struct Slime *slime)
{
    struct SceneActorMotionView *actor = (void *)slime;

    if ((s32)slime->groundHeight2BE - slime->previousGround > 0x1FF) {
        s32 cell = SceneActorTerrain_GetCellIndex(&slime->body.position);
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

u32 Slime_Update(struct SceneActorRegistryResetView *actor)
{
    u32 i;
    void **t;
    Slime_ProcessPendingStateChange(actor);
    i = actor->state2A8;
    t = actor->callbacks27C;
    Callback_InvokeActor(actor, t[i]);
    SceneActorControl_DispatchControl(actor);
    SceneActorMotion_RefreshGroundHeight(actor);
    return 1;
}

void SceneActor_UpdateActiveCallbacks(u8 *);

u32 Slime_OnMessage(struct SceneActorRegistryResetView *actor)
{
    actor->tracking.target = &((struct SceneActorChasePlayer *)gPlayers[0])->target;
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

static inline u8 Slime_TestDestroyFlags(u16 *a, u32 m)
{
    if (*a & m)
        return 1;
    return 0;
}

u32 Slime_Destroy(struct SceneActorRegistryResetView *actor)
{
    u8 *p = actor->drawable;
    u8 *q;
    u32 m;
    u32 v;
    if (actor->drawableKind == 0)
        Sprite_Remove(p);
    else
        Anim_Remove(&((struct SceneActorAnimationTileDrawable *)p)->tile);
    Heap_Free(p);
    m = 0x100;
    if (Slime_TestDestroyFlags(&actor->flags2C0, m))
        SceneMessages_UnregisterReceiver(&actor->unknown9[3]);
    {
        u8 *r = actor->shadow;
        if (*r == 1)
            Shadow_Destroy(r);
    }
    m = 0x2000000;
    v = actor->flags2D0 & m;
    q = (void *)&actor->body;
    if (v == 0)
        TerrainOverlay_QueueRemoveActor(q);

    SceneBody_Unregister(q);
    Collider_Remove(&actor->colliderA);
    SceneActorRegistry_UnlinkActor(actor);
    return 0;
}

void SceneActorRegistry_SetObjectContext(void *, void *, s32, s32);

void SceneActorReaction_OnEnemyDefault(void);

void SceneActorReaction_ResolveDamage(void);

void SceneActorReaction_SinkAndRemove(void);

void SceneActorScript_OnCommand(void);

void AnimData_SelectPaletteBank(void *, s32);

void SceneActorPlacement_HideActor(void *, s32);

void SceneActorBehavior_InstallCallbackTables(void *);

void SceneActorBehavior_InitializeShadow(void *);

void SceneActorTerrain_ReadScriptParameters(void *);

u8 SceneActorRegistry_LinkActor(void *);

extern SceneActorCommandHandler const gUnk_086108B0[];

static inline void MonsterInit_InitializeBody(struct SceneActorRegistryResetView *arg0, u32 *fill)
{
    struct SceneActorBody *body = &arg0->body;
    struct ContactPosition origin;
    s32 mode, firstLimit, secondLimit;
    s32 scriptVariant;
    arg0->flags2CC = 0;
    arg0->flags2D0 = 0;
    arg0->enabled = 0;
    arg0->field2E8 = 8;
    arg0->flags2C0 = MonsterInit_ReadKeywordHalfword(0x4D, 0);
    MonsterInit_ClearPosition(fill, &origin);
    mode = Script_GetKeywordValue(0x4E, 0);

    MonsterInit_ReadTargeting(&MonsterInit_ReadAttacks(arg0)->tracking);

    if (Script_SeekToKeyword(0x6C)) {
        firstLimit = MonsterInit_ReadWord(0x12C);
        secondLimit = MonsterInit_ReadWord(0x12C);
    } else {
        firstLimit = 0;
        secondLimit = 0;
    }

    if (Script_GetKeywordValue(0x62, 0))
        MonsterInit_SetStatusFlags(arg0, 0x200);
    arg0->field1F0 = MonsterInit_ReadKeywordHalfword(0x46, 0);
    arg0->field1EC = MonsterInit_ReadKeywordHalfword(0x52, 0);

    if (Script_SeekToKeyword(0x41)) {
        MonsterInit_SetInitialHealth(arg0, MonsterInit_ReadHalfword(0));
        arg0->field9E = MonsterInit_ReadByte(0);
        arg0->field9F = MonsterInit_ReadByte(0);
        arg0->fieldA0 = MonsterInit_ReadByte(0);
        arg0->fieldA2 = MonsterInit_ReadHalfword(0);
        arg0->fieldA4 = MonsterInit_ReadHalfword(0);
    }

    MonsterInit_InitializeHealth(arg0, MonsterInit_GetInitialHealth(arg0), firstLimit, secondLimit);

    if (Script_SeekToKeyword(0x58)) {
        arg0->fieldAC = MonsterInit_ReadWord(0);
        arg0->fieldA1 = MonsterInit_ReadByte(0);
    }
    arg0->fieldB2 = MonsterInit_ReadKeywordHalfword(0x4B, 2);
    scriptVariant = (s16)MonsterInit_ReadKeywordHalfword(0x56, -1);
    if ((s16)scriptVariant < 0)
        scriptVariant = 0xFFFF;
    arg0->field9A = scriptVariant;
    if (Script_GetKeywordValue(0x48, 0))
        MonsterInit_SetStatusFlags(arg0, 0x800);

    SceneBody_Init(body, mode, &origin, 0, 4, arg0);
    SceneBody_SetSprite(body, MonsterInit_GetDrawableKind(arg0) ? 0 : arg0->drawable);
    SceneBody_SetMovementProbe(body, ((struct Slime *)arg0)->movementProbe, 0x40, 0x40);
}

static inline void Slime_ReadPalette(struct SceneActorRegistryResetView *actor, u16 base,
                                     u16 alternate)
{
    s32 r1v;
    actor->paletteOffset = MonsterInit_ReadKeywordHalfword(0x43, 0);
    actor->basePaletteBank = base;
    actor->paletteAlternateBank = alternate;
    MonsterInit_SetAlternatePaletteOffset(actor, MonsterInit_GetPaletteOffset(actor) + 0xCC);
    r1v = MonsterInit_GetPaletteBase(actor) + MonsterInit_GetPaletteOffset(actor);
    if (MonsterInit_GetDrawableKind(actor) == 0)
        AnimData_SelectPaletteBank(((struct SlimeDrawable *)actor->drawable)->animation, r1v);
    else
        ((struct SceneActorRegistryTileDrawable *)actor->drawable)->palette =
            gPaletteBankData + (r1v << 5);
    if (MonsterInit_GetDrawableKind(actor) == 1)
        MonsterInit_SetWidgetPalette(
            &((struct SceneActorAnimationTileDrawable *)actor->drawable)->tile,
            SceneActorScript_GetManagerField62() - 0x5FFF);
}

static inline void MonsterInit_InstallPrimary(struct SceneActorRegistryResetView *actor, void *cb0,
                                              void *cb1, void *cb2, void *cb3, void *cb4,
                                              void *hidden, void *cb5, void *cb6,
                                              void *defaultReaction, void *targetQuery,
                                              void *damage, void *contact)
{
    MonsterInit_SetBehaviorFlags(actor, 0x2000);
    actor->contextCallback = cb0;
    MonsterInit_WriteCallback(&actor->animationQuery.opaque, cb1);
    actor->entryTarget224 = cb2;
    actor->entryTarget228 = cb3;
    actor->savedTarget = cb4;
    actor->reactionTarget230 = cb5;
    actor->deathControl = cb6;
    actor->field220 = hidden;
    actor->defaultReaction = defaultReaction;
    MonsterInit_WriteCallback(&actor->contactQuery.opaque, 0);
    MonsterInit_WriteCommandTable(&actor->commandTable.handlers, gUnk_086108B0);
    actor->callback248 = 0;
    actor->update26C = targetQuery;
    actor->field238 = SceneActorReaction_SinkAndRemove;
    actor->damageCallback = damage;
    actor->contactCallback = contact;
    actor->field240 = 0;
    MonsterInit_ClearMotion(actor);
}

static inline void Slime_InstallCompletionCallbacks(struct SceneActorRegistryResetView *actor)
{
    actor->messageCallback = SceneActorScript_OnCommand;
    actor->updateCallback = Slime_OnMessage;
    actor->destroyCallback = Slime_Destroy;
}

static inline void Slime_InstallRoutes(struct SceneActorRegistryResetView *actor)
{
    actor->callback248 = Slime_ResetIdlePlayback;
    actor->field23C = Slime_FollowPlayer;
    MonsterInit_WriteCallback(&actor->contactQuery.opaque, Slime_TryEnrage);
    actor->field240 = Slime_PlayPendingSound;
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

static inline s32 MonsterInit_UpdateTerrainCell(struct SceneActorRegistryResetView *actor)
{
    struct ContactPosition *position = &actor->body.position;
    actor->terrainCell = MonsterInit_CellId(position);
    return MonsterInit_TerrainHeight(MonsterInit_GetTerrainCell(actor), position);
}

s32 Slime_Init(void *object)
{
    struct Slime *slime = object;
    struct SceneActorRegistryResetView *actor = object;
    u32 zero;
    struct ContactPosition tmp;
    u32 flag;
    s32 j;
    slime->flags = 0;
    SceneActorRegistry_SetObjectContext(actor, actor, 0, 0);
    MonsterInit_SetCategory(actor, 0x16);
    Slime_InitProfile(actor);
    if (MonsterInit_GetDrawableKind(actor) == 0) {
        u8 *heap;
        actor->drawable = Heap_Alloc(0x5C);
        Memory_Clear(actor->drawable, 0x5C);
        heap = MonsterInit_Drawable(actor);
        MonsterInit_InitializeSprite(heap, 0xB96C, 0xA929);
        if (MonsterInit_GetCategory(actor) == 0x11 || MonsterInit_GetCategory(actor) == 0x17)
            heap[7] = 1;
        actor->body.direction = 0;
    } else {
        u8 *heap;
        void *res;
        actor->drawable = Heap_Alloc(0x80);
        Memory_Clear(actor->drawable, 0x80);
        heap = MonsterInit_Drawable(actor);
        MonsterInit_ClearPosition(&zero, &tmp);
        res = ScriptProc_Start(0xCB05, 0xB96C);
        *(struct DisplayResourceHeader *)heap = *(struct DisplayResourceHeader *)res;
        ResourceHeader_Relocate(heap, res);
        Widget_InitLinked((void *)&((struct SceneActorAnimationTileDrawable *)heap)->tile, heap, 0,
                          0, 2, 2, 0x3C, &tmp);
        actor->animationIndex = 0xFF;
    }
    MonsterInit_InitializeBody(actor, &zero);
    slime->halfHealth = MonsterInit_GetHealth(actor) >> 1;
    if (Script_SeekToKeyword('p')) {
        s32 ground;
        actor->body.position.x = MonsterInit_ReadHalfword(0);
        actor->body.position.y = MonsterInit_ReadHalfword(0);
        actor->body.position.z = MonsterInit_ReadHalfword(0);
        ground = MonsterInit_UpdateTerrainCell(actor);
        actor->groundHeight2BE = ground;
        slime->previousGround = ground;
        if (MonsterInit_GetDrawableKind(actor) == 0)
            ((struct SlimeDrawable *)actor->drawable)->position = actor->body.position;
        else
            ((struct SceneActorRegistryTileDrawable *)actor->drawable)->position =
                actor->body.position;
    }
    j = Script_SeekToKeyword('j');
    if (j) {
        actor->secondaryMotion.x = MonsterInit_ReadHalfword(0);
        actor->secondaryMotion.y = MonsterInit_ReadHalfword(0);
        actor->secondaryMotion.z = MonsterInit_ReadHalfword(0);
        Slime_SetFlags(slime, 0x18);
        {
            s32 health = Script_GetKeywordValue('m', 0);
            if (health)
                actor->defaultHealth = health;
        }
        actor->field2E8 = 1;
    } else {
        actor->field2E8 = j;
    }
    actor->colliderA.responseValue = MonsterInit_ReadKeywordHalfword('n', 0);
    slime->launchThreshold = 600;
    actor->field2EA = Script_GetKeywordValue('f', 0);
    if (actor->field2EA)
        Slime_SetFlags(slime, 0x10);
    Bat_ReadScriptValuePair(actor);
    Slime_ReadPalette(actor, 0xCC, 0x192);
    Slime_InitColliders(actor);
    MonsterInit_InstallPrimary(actor, Slime_Update, Slime_GetAnimationMovePulse, 0,
                               Slime_ResetAnimation, Slime_RollMoveFlag, 0,
                               Slime_ConvergeReactionMotion, Bat_RunDefeatScript,
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
        MonsterInit_MaskColliderFlags(&actor->colliderA, ~0x100);
    if (Script_GetKeywordValue(0x53, 0) != 0 || !MonsterInit_IsInCurrentRegion(actor))
        SceneActorPlacement_HideActor(actor, 0);
    SceneActorAnimation_SelectDirectionalEntry(actor, 0);
    if (MonsterInit_GetDrawableKind(actor) == 0)
        DisplaySequence_Step(actor->drawable);
    else
        Anim_Step(&((struct SceneActorAnimationTileDrawable *)actor->drawable)->tile,
                  actor->drawable);
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
