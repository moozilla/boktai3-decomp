#include "mimic.h"

static inline void SaveControl(struct MimicDeferredResponse *packet, void *control, u8 behavior)
{
    packet->delay = 30;
    packet->control = control;
    packet->state = 0;
    packet->phase = 0;
    packet->behavior = behavior;
}

static inline void ClearContextFlag(u16 *flags, u32 zero)
{
    s32 mask = zero - 2;
    mask &= *flags;
    *flags = mask;
}

static inline u32 SaveStoppedControl(struct MimicDeferredResponse *packet, void *control)
{
    u32 zero;
    packet->delay = 30;
    packet->control = control;
    zero = 0;
    packet->state = zero;
    packet->phase = zero;
    packet->behavior = zero;
    return zero;
}

static inline void clearRouteFlag(u32 *flags)
{
    u32 mask = ~0x1000000;
    *flags &= mask;
}

static inline u32 testFlags(struct SceneActorRegistryResetView *actor, u32 mask)
{
    return actor->flags2CC & mask;
}

static inline void ClearSavedFlags(u32 *flags, s32 mask)
{
    *flags &= mask;
}

static inline void *UpdateCallback(struct SceneActorRegistryResetView *actor)
{
    return actor->contactQuery.opaque;
}

static inline u8 Mimic_FinishRoute(struct SceneActorRegistryResetView *actor, s32 result,
                                   void *route, struct ContactPosition *position,
                                   struct ContactPosition *target)
{
    if (!result) {
        MimicChase_ClearBits(&actor->flags2D4, 0x200);
        return 0;
    }
    result = Terrain_ApplyBlocked(route, position, target);
    if (result) {
        if (result != 2)
            Memory_Clear(&actor->body.motion, 8);
        return 1;
    }
    MimicChase_ClearBits(&actor->flags2D4, 0x200);
    return 0;
}

static inline void Mimic_SaveRouteControl(struct SceneActorRegistryResetView *actor,
                                          struct Mimic *context, struct SceneActorChaseGroup *group,
                                          u32 *flags)
{
    u8 outside = SceneActorAttachment_TestPositionWindow((void *)actor, &context->origin, 32, 32);
    if (!outside) {
        ClearSavedFlags(flags, ~0xc0);
        SaveControl((struct MimicDeferredResponse *)&group->saved, actor->entryTarget228, 2);
    } else {
        u32 zero;
        ClearSavedFlags(flags, ~0xc0);
        zero =
            SaveStoppedControl((struct MimicDeferredResponse *)&group->saved, actor->savedTarget);
        ClearContextFlag(&context->reactionFlags, zero);
    }
    actor->field2C2 = 0x40;
}

// confidence=low: provisional name; recovery after a hit (state 3 -> delayed restore), Mimic-owned.

void Mimic_UpdateHitRecovery(struct SceneActorRegistryResetView *actor)
{
    struct Mimic *context = actor->context;
    if (MimicChase_TakeArrived(actor)) {
        struct SceneActorReactionMotion *motion;
        struct SceneActorRegistryControl *control;
        u16 *flags;
        motion = (void *)&actor->tracking.target;
        motion->fieldDC = 0;
        {
            u16 *h;
            u32 v;
            h = &actor->field2C2;
            v = 0x40;
            *h = v;
        }
        {
            u16 *field;
            field = (u16 *)&actor->group.common.gate.flags;
            *field = 0;
        }
        {
            u16 *h;
            h = &actor->group.common.gate.counter;
            *h = 0;
        }
        flags = &actor->flags2D4;
        {
            u32 mask = ~0x380;
            mask &= *flags;
            *flags = mask;
        }
        {
            s32 mask = ~0x18;
            mask &= motion->flags;
            motion->flags = mask;
        }
        control = &actor->group.common.control;
        if (testFlags(actor, 0x200000)) {
            clearRouteFlag(&actor->flags2D0);
            {
                u32 mask = ~0x200;
                mask &= *flags;
                *flags = mask;
            }
            control->mode = 0;
            control->counter = 0;
            control->timer = 0;
        }
        {
            u32 mask = ~0x400;
            mask &= *flags;
            *flags = mask;
        }
        actor->group.turn.turnsRemaining = 0xFF;
    }
    if ((s32)actor->counter2AC > 30) {
        struct SceneActorChaseGroup *group = &actor->group;
        u32 *cc = &actor->flags2CC;
        ClearSavedFlags(cc, ~0x100);
        {
            struct Mimic *current = actor->context;
            u16 mask = 1;
            struct MimicReactionFlags *mode = (void *)&current->reactionFlags;
            mode->bits |= mask;
        }
        SceneActorAttachment_ReplaceResponseSlot(actor, 0, 2, actor->category);
        MimicChase_SetState(actor, 3, 1, 0);
        SceneActorChase_SetControl(actor, SceneActorReaction_RestoreAfterDelay, 14);
        {
            void *route = actor->group.route;
            struct ContactPosition *position = &actor->body.position;
            struct ContactPosition *target = &context->origin;
            s32 result = Nav_IsPositionInMesh(route, position, target);
            u32 *savedFlags = cc;
            if (Mimic_FinishRoute(actor, result, route, position, target))
                Mimic_SaveRouteControl(actor, context, group, savedFlags);
        }
    } else {
        Callback_InvokeActor(actor, UpdateCallback(actor));
        actor->counter2AC++;
    }
}

void Mimic_InitBehaviorTable(struct SceneActorRegistryResetView *actor)
{
    actor->callbacks284 = (void **)gUnk_08606264;
}

s32 Mimic_UpdateReactionState(struct SceneActorRegistryResetView *actor)
{
    Mimic_ResetExpiredTarget(actor);
    return 1;
}

/* SceneActor_EnterFlaggedEntryPhase: plays directional entry 16 once, clears the 0x4 bit, and forwards the restart flag */

void SceneActor_EnterFlaggedEntryPhase(struct SceneActorAnimationView *actor)
{
    if (SceneActorControl_TakeFlag(&actor->directionalEntryFlag)) {
        u32 flags = SceneActor_GetDirectionFlags(actor->direction);
        struct ContactCollider *collider;
        SceneActorAnimation_SelectEntryFlags(actor, 16, flags);
        collider = &((struct SceneActorRegistryResetView *)actor)->colliderB;
        collider->flags &= 0xFFFB;
    }
    if (actor->restart)
        actor->unknown2B0 = 1;
}

static inline void CellCenter(struct ContactPosition *out, u32 cell)
{
    s32 x = Mod(cell, ((struct SceneActorChaseTerrainGrid *)gTerrainMap->cells)->width);
    s32 z = Div(cell, ((struct SceneActorChaseTerrainGrid *)gTerrainMap->cells)->width);
    u8 *data;
    out->x = (x << 8) + 0x80;
    data = TerrainModifiers_FindCellOverride(cell, 1);
    if (data)
        data += 4;
    else
        data = gTerrainMap->cells + (cell * 4 + 12);
    out->y = (*data & 15) << 8;
    out->z = (z << 8) + 0x80;
}

static inline s32 Scale12(s32 n)
{
    if (n >= 0)
        return n >> 12;
    return -((-n) >> 12);
}

static inline void Polar(struct ContactPosition *out, u32 angle, s32 speed)
{
    out->x = Scale12(gUnk_086149C4[(angle + 0x40) & 0xFF] * speed);
    out->y = 0;
    out->z = Scale12(gUnk_086149C4[angle & 0xFF] * speed);
}

static inline void Delta(struct ContactPosition *out, const struct ContactPosition *from,
                         const struct ContactPosition *to)
{
    out->x = to->x - from->x;
    out->y = to->y - from->y;
    out->z = to->z - from->z;
}

static inline void RouteDelta(struct ContactPosition *out, struct ContactPosition *from,
                              struct SceneActorChaseGroup *group)
{
    Delta(out, from, &((struct MimicNavigationProbe *)group->route)->destination);
}

static inline u8 HasMode(struct Mimic *p)
{
    u32 mask = 1;
    if (p->reactionFlags & mask)
        return 1;
    return 0;
}

static inline s32 SpeedBase(struct Mimic *context)
{
    return context->leapSpeed - 2;
}

// confidence=low: generic scene actor leap; species ownership is not established.

void SceneActor_LeapTowardTarget(struct SceneActorRegistryResetView *actor)
{
    struct Mimic *context = actor->context;
    struct SceneActorChaseGroup *group = &actor->group;
    struct ContactPosition point, delta;
    const u16 *samples = gRandomTable;
    if (SceneActorControl_TakeFlag(&actor->field2B2)) {
        s32 correcting;
        s32 speed;
        if (!HasMode(context)) {
            if (!Mimic_TryTargetRoute(actor->context)) {
                SceneActor_ResetCollisionReaction(actor);
                return;
            }
        } else if (!Mimic_TryPursuitRoute(actor->context))
            return;
        CellCenter(&point, actor->terrainCell);
        Delta(&point, &actor->body.position, &point);
        {
            s32 quadrant = ((actor->body.direction + 0x20) & 0xFF) >> 6;
            correcting = 0;
            switch (quadrant) {
                case 0:
                case 2:
                    if ((point.z < 0 ? -point.z : point.z) > 0x40) {
                        CellCenter(&point, actor->terrainCell);
                        point.x = actor->body.position.x;
                        correcting = 1;
                    }
                    break;
                case 1:
                case 3:
                    if ((point.x < 0 ? -point.x : point.x) > 0x40) {
                        CellCenter(&point, actor->terrainCell);
                        point.z = actor->body.position.z;
                        correcting = 1;
                    }
                    break;
            }
        }
        if (correcting) {
            Delta(&delta, &actor->body.position, &point);
            speed = 4;
        } else {
            RouteDelta(&delta, &actor->body.position, group);
            gRandomIndex = (gRandomIndex + 1) & 1023;
            {
                u32 random = samples[gRandomIndex];
                s32 base = SpeedBase(context);
                speed = base + (random & 3);
            }
        }
        {
            s32 angle = Math_Atan2(delta.x, delta.z);
            u8 *heading = &actor->body.direction;
            *heading = angle;
            {
                Polar(&actor->secondaryMotion, *heading, speed);
            }
        }
        actor->secondaryMotion.y = 24;
        SceneActorAnimation_SelectEntryFlags((void *)actor, 14,
                                             SceneActor_GetDirectionFlags(actor->body.direction));
        Sound_PlaySong(0x1B2);
        ActorShadow_Enable(actor->shadow);
    } else {
        u32 mask = 16;
        if (actor->flags2D0 & mask)
            actor->field2B0 = 1;
    }
}

void SceneActor_EnterShadowlessPhase(struct SceneActorAnimationView *actor)
{
    if (SceneActorControl_TakeFlag(&actor->directionalEntryFlag)) {
        if (SceneActor_IsCollisionTargetUnavailable(actor)) {
            u32 flags = SceneActor_GetDirectionFlags(actor->direction);
            SceneActorAnimation_SelectEntryFlags(actor, 15, flags);
        } else {
            u32 flags = SceneActor_GetDirectionFlags(actor->direction);
            SceneActorAnimation_SelectEntryFlags(actor, 16, flags);
        }
        ActorShadow_Disable(((struct SceneActorRegistryResetView *)actor)->shadow);
    }
    if (actor->restart)
        actor->unknown2B0 = 1;
}

void SceneActor_RestoreSavedTargetOnCompletion(struct SceneActorRegistryResetView *actor)
{
    if (SceneActorControl_TakeFlag(&actor->field2B2)) {
        u32 flags = SceneActor_GetDirectionFlags(actor->body.direction);
        struct ContactCollider *collider;
        SceneActorAnimation_SelectEntryFlags(actor, 0, flags);
        collider = &actor->colliderA;
        collider->flags = 4 | collider->flags;
    }
    {
        u32 mask = 0x10;
        if (actor->flags2D0 & mask) {
            u32 zero;
            void *saved;
            struct ContactCollider *collider = &actor->colliderA;
            collider->flags &= 0xFFFB;
            zero = 0;
            saved = actor->savedTarget;
            actor->field2B2 = 1;
            actor->field2B0 = zero;
            actor->field2B5 = zero;
            actor->target = saved;
            actor->field2C8 = zero;
            {
                u32 *flags = &actor->flags114;
                u32 clear = ~1;
                *flags &= clear;
            }
            actor->word104 = zero;
            actor->state2A8 = zero;
            actor->phase2A9 = zero;
            actor->behavior2AA = zero;
            actor->counter2AC = zero;
            actor->field2B1 = 1;
        }
    }
}

void Mimic_OnAnimationStateChanged(struct SceneActorRegistryResetView *actor)
{
    if (SceneActorControl_TakeFlag(&actor->field2B2)) {
        u32 r = SceneActor_GetDirectionFlags(actor->body.direction);
        struct ContactCollider *w;
        SceneActorAnimation_SelectEntryFlags(actor, 0, r);
        w = &actor->colliderB;
        w->flags = 4 | w->flags;
    }
}

void SceneActor_WaitForCategoryAttachment(struct SceneActorRegistryResetView *actor, s32 frame)
{
    if (SceneActorControl_TakeFlag(&actor->field2B2)) {
        u32 flags = SceneActor_GetDirectionFlags(actor->body.direction);
        struct ContactCollider *collider;
        SceneActorAnimation_SelectEntryFlags(actor, 0, flags);
        collider = &actor->colliderB;
        collider->flags = 4 | collider->flags;
    }
    if (frame == 160) {
        SceneActorAttachment_ReplaceResponseSlot(actor, 0, 2, actor->category);
    } else if (frame > 160) {
        if ((u8)ActorAttachment_IsUnavailable(actor->group.attachment))
            actor->field2B0 = 1;
    }
}

static inline u8 DrawableVisible(struct SceneActorRegistryResetView *actor)
{
    u32 mask = 1;
    if (!actor->drawableKind) {
        if (((struct ActorSprite *)actor->drawable)->flags & mask)
            return 1;
        return 0;
    }
    if (((struct SceneActorRegistryTileDrawable *)actor->drawable)->flags & mask)
        return 1;
    return 0;
}

static inline void SetState(struct SceneActorRegistryResetView *actor, u32 state, u32 phase,
                            u32 behavior)
{
    actor->state2A8 = state;
    actor->phase2A9 = phase;
    actor->behavior2AA = behavior;
    actor->counter2AC = 0;
    actor->field2B1 = 1;
}

static inline void ResetElapsed(struct DisplaySequenceState *sequence)
{
    sequence->elapsed = 0;
}

// confidence=low: Mimic defeat blink inferred from neighboring behavior callbacks.

void Mimic_UpdateDefeatBlink(struct SceneActorRegistryResetView *actor, s32 frame)
{
    s32 values[8];
    struct ContactPackedVector range;
    struct ScriptUtilityArguments arguments;
    if (SceneActorReaction_TakePrepared(actor))
        SceneActorAnimation_SelectEntryFlags((void *)actor, 10,
                                             SceneActor_GetDirectionFlags(actor->body.direction));
    ResetElapsed(((struct SceneActorAnimationSequenceDrawable *)actor->drawable)->sequence);
    if (frame <= 19)
        return;
    if (!(frame & 1)) {
        range.z = 128;
        range.x = 128;
        range.y = 64;
        SparkParticle_Spawn(&actor->body.position, 1, &range, 0, 0, 0, 2, 2);
        if (!(frame & 3))
            Sound_PlaySong(0x16E);
    }
    if (Sprite_BlinkUntilExpired(actor->drawable, frame, 10, 30)) {
        u16 script;
        SceneActorChase_SetControl(actor, SceneActorControl_OnIdle, 16);
        SetState(actor, 1, 0, 2);
        ((struct ActorSprite *)actor->drawable)->flags |= 1;
        ActorShadow_Disable(actor->shadow);
        SceneActorBehavior_CallTerrainScript(actor);
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
            u32 m = 0x4000;
            if (!(actor->flags2D4 & m) && (u32)(actor->category - 11) > 2)
                Bestiary_UnlockEntry(
                    Bestiary_GetEntryForActor(actor->category, actor->paletteOffset));
        }
    } else if (DrawableVisible(actor))
        ActorShadow_Disable(actor->shadow);
    else
        ActorShadow_Enable(actor->shadow);
}

static inline u8 AtRest(s32 x, s32 z)
{
    if (!x && !z)
        return 1;
    return 0;
}

static inline void SetFlags(u16 *flags, u16 mask)
{
    *flags = mask | *flags;
}

static inline void ClearBits(u16 *field, u32 bits)
{
    *field &= ~bits;
}

static inline void SetBehavior(struct SceneActorRegistryResetView *actor, u32 b)
{
    actor->state2A8 = 0;
    actor->phase2A9 = 0;
    actor->behavior2AA = b;
    actor->counter2AC = 0;
    actor->field2B1 = 1;
}

void Mimic_ConvergeAndTurn(struct SceneActorRegistryResetView *actor)
{
    s16 *px, *pz;
    s32 x, z;
    if (SceneActorControl_TakeFlag(&actor->field2B2)) {
        SceneActorAnimation_SelectEntryFlags((void *)actor, 3,
                                             SceneActor_GetDirectionFlags(actor->body.direction));
        {
            u16 mask = 0x10;
            u16 *flags = &actor->flags2D6;
            *flags = mask | *flags;
        }
    }
    x = SceneActorDisplacement_ReadAxis(actor->reactionStart.x, &actor->reactionEnd.x, &px);
    z = SceneActorDisplacement_ReadAxis(actor->reactionStart.z, &actor->reactionEnd.z, &pz);
    if (x > 200)
        x = 200;
    else if (x < -200)
        x = -200;
    if (z > 200)
        z = 200;
    else if (z < -200)
        z = -200;
    actor->body.motion.x += x;
    actor->body.motion.z += z;
    *px += x;
    *pz += z;
    if (AtRest(x, z) && actor->field2B3) {
        u16 *motion = &actor->flags2D4;
        u16 *flags;
        u32 hit;
        struct SceneActorChaseTracking *tracking;
        ClearBits(motion, 0x20);
        hit = gEngineFlags & 0x4800;
        flags = motion;
        if (SceneActorChase_Test(hit, hit)) {
            if (!SceneActorTarget_TrySelectNearestParticipant(actor)) {
                SceneActor_ResetCollisionReaction(actor);
                return;
            }
        } else {
            actor->tracking.target = (void *)&gPlayers[0]->positionalEffectGroup;
        }
        tracking = &actor->tracking;
        if (tracking->target) {
            struct ContactPosition d;
            Delta(&d, &actor->body.position, &tracking->target->position);
            actor->body.direction = actor->tracking.heading = Math_Atan2(d.x, d.z);
            SetFlags(&actor->group.common.gate.flags, 4);
            SetFlags(flags, 0x80);
            actor->group.common.gate.counter = actor->group.common.gate.duration;
            ClearBits(&((struct Mimic *)actor->context)->reactionFlags, 1);
            {
                u8 mask = 8;
                tracking->flags |= mask;
            }
            SetBehavior(actor, 2);
        } else {
            SetBehavior(actor, 0);
        }
    }
    {
        u32 mask = 0x2000;
        actor->flags2CC = mask | actor->flags2CC;
    }
}

static inline u32 Cell(struct ContactPosition *p)
{
    s32 x = p->x >> 8, z = p->z >> 8;
    if (x < 0 || z < 0 || (u32)x >= gTerrainWidth || (u32)z >= gTerrainHeight)
        return 0;
    return gTerrainMap->rows[z] + x;
}

// confidence=low: resets Mimic pursuit state and advances its idle drawable.

void Mimic_ResetPursuitDrawable(struct SceneActorRegistryResetView *actor)
{
    struct Mimic *context = actor->context;
    context->timer = 0;
    context->reactionFlags = 0;
    if (SceneActorChase_Test(gEngineFlags, 0x4800)) {
        context->origin = actor->body.position;
        context->originCell = Cell(&actor->body.position);
    }
    SceneActorAnimation_SelectEntryFlags((void *)actor, 0,
                                         SceneActor_GetDirectionFlags(actor->body.direction));
    {
        struct SceneActorControlMotionDrawable *drawable = actor->drawable;
        DisplaySequence_StepState(drawable, &drawable->sequence);
    }
}

void Mimic_OnAttackContact(u8 *unused, struct ContactCollider *obj,
                           struct SceneActorRegistryResetView *ctx)
{
    int mask;
    u32 *flags;
    if ((SceneActor_ActionsBlocked(ctx) == 0) && (obj->contactFlags & 0xF)) {
        ctx->field304 = obj->collisionId;
        flags = &ctx->flags2D0;
        mask = 0xFEFFFFFF;
        *flags &= mask;
    }
}

void Mimic_OnBodyContact(void *a, s32 b)
{
    SceneActorReaction_HandleColliderContact(a, b);
}

void Mimic_InitProfile(struct SceneActorRegistryResetView *actor)
{
    actor->profileId = SceneActorProfiles_FindOrAdd(actor->category, 0);
}

static inline void Mimic_ClearRouteFlag(u32 *flags)
{
    u32 mask = ~0x1000000;
    *flags &= mask;
}

static inline u32 Mimic_TestFlags(struct SceneActorRegistryResetView *actor, u32 mask)
{
    return actor->flags2CC & mask;
}

// confidence=low: drops the acquired target when its response cooldown expires.

void Mimic_ResetExpiredTarget(struct SceneActorRegistryResetView *actor)
{
    u16 *flags;
    u16 *timer;
    u32 activeMask = 0x80;
    flags = &actor->flags2D4;
    if (!(activeMask & *flags))
        return;
    timer = &actor->group.common.gate.counter;
    if (!*timer && !(*flags & 0x38)) {
        struct SceneActorReactionMotion *motion;
        struct SceneActorRegistryControl *control;
        motion = (void *)&actor->tracking.target;
        motion->fieldDC = 0;
        actor->field2C2 = 0x40;
        actor->group.common.gate.flags = 0;
        *timer = 0;
        {
            u32 mask = ~0x380;
            mask &= *flags;
            *flags = mask;
        }
        {
            s32 mask = ~0x18;
            mask &= motion->flags;
            motion->flags = mask;
        }
        control = &actor->group.common.control;
        if (Mimic_TestFlags(actor, 0x200000)) {
            Mimic_ClearRouteFlag(&actor->flags2D0);
            {
                u32 mask = ~0x200;
                mask &= *flags;
                *flags = mask;
            }
            control->mode = 0;
            control->counter = 0;
            control->timer = 0;
        }
        {
            u32 mask = ~0x400;
            mask &= *flags;
            *flags = mask;
        }
        actor->group.turn.turnsRemaining = 0xFF;
        SceneActorChase_SetControl(actor, SceneActorControl_OnIdle, 16);
        MimicChase_SetState(actor, 0, 0, 17);
    }
}

void SceneActor_InitializeReactionColliders(void *p)
{
    union {
        struct ContactPackedVector packed;
        struct ContactPosition position;
    } bounds, offset;
    bounds.packed.x = 50;
    bounds.packed.y = 30;
    bounds.packed.z = 50;
    offset.packed.x = 0;
    offset.packed.y = 40;
    offset.packed.z = 0;
    SceneActorCollision_InitAttack(p, &bounds.position, &offset.position, Mimic_OnAttackContact, 0,
                                   p);
    bounds.position.x = 80;
    bounds.position.y = 120;
    bounds.position.z = 80;
    offset.position.x = 0;
    offset.position.y = 120;
    offset.position.z = 0;
    SceneActorCollision_InitBody(p, &bounds.position, &offset.position, Mimic_OnBodyContact, p);
}

static inline s32 Square(s32 x)
{
    return x * x;
}

u8 SceneActor_FindNearbyActivePlayer(struct SceneActorRegistryResetView *p)
{
    s32 i = 0;
    u32 excluded = 8;
    struct ContactPosition difference;
    struct ContactPosition *delta = &difference;
    struct ContactPosition *position = &p->body.position;
    struct Player **cursor = gPlayers;
    do {
        u32 indexOffset = i << 2;
        struct Player *player = *cursor;
        if (player && !(*(u32 *)((u32)gPlayerSlotStatus + indexOffset) & excluded)) {
            struct SceneVec *source = &player->position;
            s32 x, z;
            s16 new_var;
            delta->x = (u16)player->position.x - (u16)position->x;
            delta->y = (u16)source->y - (u16)position->y;
            delta->z = (u16)source->z - (u16)position->z;
            x = Square(delta->x);
            new_var = delta->z;
            z = Square(new_var);
            if (x + z <= 0x2400) {
                struct Player *nearby = *cursor;
                if (nearby->inputKeys[1] & 1) {
                    struct SceneActorReactionMotion *motion = (void *)&p->tracking.target;
                    motion->fieldDC = (u32)&nearby->positionalEffectGroup;
                    return 1;
                }
            }
        }
        cursor++;
        i++;
    } while (i <= 3);
    return 0;
}

static inline s32 PlayerSlot(struct SceneActorChaseTarget *actor)
{
    if (actor->playerIndex <= 3)
        return actor->playerIndex;
    return -1;
}

static inline void ClearFlags16(u16 *p, u32 mask)
{
    *p &= mask;
}

static inline u8 TryMove(struct SceneActorRegistryResetView *actor,
                         struct SceneActorChaseGroup *group, struct SceneActorChaseTarget **slot)
{
    void *route = group->route;
    struct ContactPosition *position = &actor->body.position;
    struct ContactPosition *destination = &(*slot)->position;
    s32 result;
    if (!Nav_IsPositionInMesh(route, position, destination)) {
        ClearFlags16(&actor->flags2D4, ~0x200);
        return 0;
    } else {
        result = Terrain_ApplyBlocked(route, position, destination);
        if (result) {
            if (result != 2)
                Memory_Clear(&actor->body.motion, 8);
            return 1;
        } else {
            ClearFlags16(&actor->flags2D4, ~0x200);
            return 0;
        }
    }
}

u8 Mimic_TryTargetRoute(struct SceneActorRegistryResetView *actor)
{
    struct SceneActorChaseGroup *group = &actor->group;
    struct SceneActorChaseTarget **slot;
    if (SceneActorChase_Test(gEngineFlags, 0x4800)) {
        struct SceneActorChaseTarget *partner;
        s32 index;
        struct SceneActorChaseTarget **p = &actor->tracking.target;
        partner = *p;
        slot = p;
        if (!partner)
            goto fail;
        index = PlayerSlot(partner);
        if (index < 0)
            goto fail;
        {
            u32 mask = 8;
            if (!(gUnk_02000560[index] & mask))
                goto join;
            goto fail;
        }
    } else {
        struct SceneActorChaseTarget *partner;
        struct SceneActorChaseTarget **p = &actor->tracking.target;
        partner = *p;
        slot = p;
        if (!partner)
            goto fail;
    }
join:
    if (TryMove(actor, group, slot))
        return 1;
fail:
    return 0;
}

// confidence=low: retries the saved pursuit goal and selects idle behavior on failure.

u8 Mimic_TryPursuitRoute(struct SceneActorRegistryResetView *actor)
{
    u32 output;
    u32 result = MimicChase_TryRoute(actor, actor->group.route, &actor->body.position,
                                     &((struct Mimic *)actor)->origin);
    if (result)
        output = 1;
    else {
        u32 behavior = 17;
        actor->state2A8 = result;
        actor->phase2A9 = result;
        actor->behavior2AA = behavior;
        actor->counter2AC = result;
        actor->field2B1 = 1;
        output = 0;
    }
    return output;
}

void Mimic_BeginTargetResponse(struct SceneActorRegistryResetView *actor)
{
    u16 *h;
    u16 value;
    s32 off;
    s32 mask;
    u16 *p;
    {
        u32 byte = actor->tracking.heading;
        actor->body.direction = byte;
    }
    SceneActorAttachment_ReplaceResponseSlot((u8 *)actor, 0, 0, actor->category);
    SceneActorBehavior_CallActionScript((u8 *)actor);
    h = (u16 *)&actor->group.common.gate.flags;
    {
        u32 flag = 4;
        flag |= *h;
        *h = flag;
    }
    {
        u32 flag = 0x80;
        u16 *flags = &actor->flags2D4;
        flag |= *flags;
        *flags = flag;
    }
    value = actor->group.common.gate.duration;
    actor->group.common.gate.counter = value;
    p = &((struct Mimic *)actor->context)->reactionFlags;
    mask = -2;
    mask &= *p;
    *p = mask;
}

/* Every quadrant has a case, but the switch keeps a default path that falls off the end with no
 * return statement; r0 still holds direction there, as in the ROM. */

u32 SceneActor_GetDirectionFlags(s32 direction)
{
    switch (((direction + 16) & 255) >> 6) {
        case 0:
        case 2:
            return 0;
        case 1:
        case 3:
            return 1;
    }
}

/* Mimic_UpdateIdleState: Empty hook near Mimic behavior helpers with idle role tentative */

void Mimic_UpdateIdleState(void)
{
}

static inline u8 motionFlag(struct SceneActorRegistryResetView *actor)
{
    struct SceneActorChaseTracking *motion = &actor->tracking;
    u32 mask = 2;
    if (motion->flags & mask)
        return 1;
    return 0;
}

static inline u8 nearFlag(struct SceneActorRegistryResetView *actor)
{
    struct SceneActorChaseTracking *motion = &actor->tracking;
    u32 mask = 8;
    if (motion->flags & mask)
        return 1;
    return 0;
}

static inline u8 collisionMode(void)
{
    if (gEngineFlags & 0x4800)
        return 1;
    return 0;
}

static inline void setTargetFlags(struct SceneActorRegistryResetView *actor)
{
    u32 mask = 0x80;
    u16 *flags = &actor->flags2D6;
    struct SceneActorChaseTracking *motion;
    mask |= *flags;
    *flags = mask;
    motion = &actor->tracking;
    {
        u32 mask = 0x10;
        mask |= motion->flags;
        motion->flags = mask;
    }
}

s32 SceneActor_AcquireCollisionTarget(struct SceneActorRegistryResetView *actor)
{
    struct SceneActorChaseGroup *group = &actor->group;
    struct SceneActorChaseTarget *target;
    s32 d;
    if (!motionFlag(actor))
        goto zero;
    if ((s32)actor->field304 > 0) {
        setTargetFlags(actor);
        actor->tracking.target = (u32)SceneActorContacts_FindObjectById((u16)actor->field304);
        return 1;
    }
    if (collisionMode()) {
        if (SceneActor_FindNearbyActivePlayer(actor))
            goto yes;
        if (!SceneActor_IsCollisionTargetUnavailable(actor))
            goto collision;
        goto zero;
    } else {
        d = actor->body.position.y - gPlayers[0]->position.y;
        if (d < 0)
            d = -d;
        if (d > 0x200)
            goto zero;
        if (!SceneActor_FindNearbyActivePlayer(actor))
            goto collision;
    }
yes:
    setTargetFlags(actor);
    return 1;
collision:
    target = actor->tracking.target;
    if (!target)
        return 0;
    if (actor->tracking.distance >= actor->tracking.limit)
        goto zero;
    if (!Nav_IsPositionInMesh(group->route, &actor->body.position, &target->position))
        goto zero;
    if (!nearFlag(actor))
        goto zero;
    setTargetFlags(actor);
zero:
    return 0;
}

static inline u32 actorFlags(struct SceneActorRegistryResetView *actor, u32 mask)
{
    return actor->flags2D4 & mask;
}

static inline u8 reactionStarted(struct SceneActorRegistryResetView *actor,
                                 struct SceneActorReactionMotion **out)
{
    struct SceneActorReactionMotion *motion = (void *)&actor->tracking.target;
    u32 mask = 8;
    u32 flags = motion->flags & mask;
    *out = motion;
    if (flags)
        return 1;
    return 0;
}

static inline s32 motionSquared(struct ContactPosition *motion)
{
    return motion->x * motion->x + motion->y * motion->y + motion->z * motion->z;
}

static inline void clearFlag(u32 *flags)
{
    u32 mask = ~1;
    *flags &= mask;
}

static inline void prepareControl(struct SceneActorRegistryResetView *actor, void *control, u8 kind,
                                  s32 zero)
{
    actor->field2B2 = 1;
    actor->field2B0 = zero;
    actor->field2B5 = kind;
    actor->target = control;
    actor->field2C8 = zero;
    clearFlag(&actor->flags114);
    actor->word104 = zero;
}

void SceneActor_PrepareStationaryReaction(struct SceneActorRegistryResetView *actor)
{
    struct SceneActorReactionMotion *motion;
    s32 offset;
    u8 started;
    if (actorFlags(actor, 0x80)) {
        actor->group.common.gate.counter = actor->group.common.gate.duration;
        return;
    }
    started = reactionStarted(actor, &motion);
    if (started)
        return;
    if (motionSquared(&actor->secondaryMotion) || (offset = *(s16 *)&actor->field2EA)) {
        Mimic_BeginTargetResponse(actor);
    } else {
        prepareControl(actor, SceneActorControl_OnIdle, 0x10, offset);
        actor->state2A8 = 0;
        actor->phase2A9 = 1;
        actor->behavior2AA = 0;
        actor->counter2AC = offset;
        actor->field2B1 = 1;
    }
    {
        u32 mask = 8;
        mask |= motion->flags;
        motion->flags = mask;
    }
}

// confidence=low: tests whether the collision target is absent or its player is unavailable.

u8 SceneActor_IsCollisionTargetUnavailable(struct SceneActorRegistryResetView *actor)
{
    struct SceneActorChaseTarget *target = actor->tracking.target;
    if (target) {
        if (target->playerIndex <= 3) {
            struct Player *player = gPlayers[target->playerIndex];
            u32 mask = 0x20000;
            if (player->statusFlags & mask)
                return 1;
            {
                u32 mask8 = 8;
                u32 flags = gPlayerSlotStatus[target->playerIndex];
                if (flags & mask8)
                    return 1;
            }
        }
        return 0;
    }
    return 1;
}

s32 Mimic_OnBehaviorComplete(void)
{
    return 0;
}

static inline void resetState(struct SceneActorRegistryResetView *actor, u8 behavior)
{
    actor->state2A8 = 0;
    actor->phase2A9 = 0;
    actor->behavior2AA = behavior;
    actor->counter2AC = 0;
    actor->field2B1 = 1;
}

static inline void setControl(struct SceneActorRegistryResetView *actor, void *control, u8 kind)
{
    actor->field2B2 = 1;
    actor->field2B0 = 0;
    actor->field2B5 = kind;
    actor->target = control;
    actor->field2C8 = 0;
    clearFlag(&actor->flags114);
    actor->word104 = 0;
}

void SceneActor_ResetCollisionReaction(struct SceneActorRegistryResetView *actor)
{
    struct SceneActorReactionMotion *motion;
    struct SceneActorRegistryControl *control;
    u16 *flags;
    resetState(actor, 0x11);
    setControl(actor, SceneActorControl_OnIdle, 0x10);
    motion = (void *)&actor->tracking.target;
    motion->fieldDC = 0;
    actor->field2C2 = 0x40;
    actor->group.common.gate.flags = 0;
    actor->group.common.gate.counter = 0;
    flags = &actor->flags2D4;
    {
        u32 mask = ~0x380;
        mask &= *flags;
        *flags = mask;
    }
    {
        s32 mask = ~0x18;
        mask &= motion->flags;
        motion->flags = mask;
    }
    control = &actor->group.common.control;
    if (testFlags(actor, 0x200000)) {
        clearRouteFlag(&actor->flags2D0);
        {
            u32 mask = ~0x200;
            mask &= *flags;
            *flags = mask;
        }
        control->mode = 0;
        control->counter = 0;
        control->timer = 0;
    }
    {
        u32 mask = ~0x400;
        mask &= *flags;
        *flags = mask;
    }
    actor->group.turn.turnsRemaining = 0xFF;
}

static inline void ResetState(struct SceneActorRegistryResetView *actor, u32 state, u32 phase)
{
    actor->state2A8 = state;
    actor->phase2A9 = phase;
    actor->behavior2AA = 0;
    actor->counter2AC = 0;
    actor->field2B1 = 1;
}

u8 Mimic_StartHitReaction(struct SceneActorRegistryResetView *actor)
{
    struct Mimic *ctx = actor->context;
    if (actor->reactionSoundVariant == 0)
        Sound_PlaySong(0x211);
    else
        Sound_PlaySong(0x13E);
    if (actor->field2DC & 0xC0000) {
        SceneActorChase_SetControl(actor, actor->reactionTarget230, 0x12);
        ResetState(actor, 3, 0);
        return 1;
    }
    ClearBits(&actor->flags2D4, 0x20);
    if (actor->tracking.target) {
        if (HasMode(actor->context)) {
            {
                u16 mask = 0x80;
                u16 *flags = &actor->flags2D6;
                *flags = mask | *flags;
            }
            ClearBits(&((struct Mimic *)actor->context)->reactionFlags, 1);
        } else if (actor->health < ctx->healthThreshold) {
            {
                u16 mask = 0x80;
                u16 *flags = &actor->flags2D6;
                *flags = mask | *flags;
            }
            ClearBits(&((struct Mimic *)actor->context)->reactionFlags, 1);
        }
    }
    return 0;
}

// confidence=low: action callback resets response and selects behavior two on entry.

void Mimic_BeginActionResponse(struct SceneActorRegistryResetView *actor)
{
    if (MimicChase_TakeArrived(actor)) {
        Mimic_BeginTargetResponse(actor);
        {
            u32 behavior = 2;
            actor->state2A8 = 0;
            actor->phase2A9 = 0;
            actor->behavior2AA = behavior;
            actor->counter2AC = 0;
            actor->field2B1 = 1;
        }
    } else
        actor->counter2AC++;
}

void Mimic_InitActionTable(struct SceneActorRegistryResetView *actor)
{
    actor->alternateBehaviorCallbacks = (void **)gUnk_086062AC;
}

s32 Mimic_UpdateBehavior(struct SceneActorRegistryResetView *actor)
{
    Callback_InvokeActor(actor, actor->callbacks27C[actor->state2A8]);
    SceneActorControl_DispatchControl(actor);
    Mimic_UpdateReactionState(actor);
    return 1;
}

s32 Mimic_OnUpdate(struct SceneActorRegistryResetView *actor)
{
    SceneActor_UpdateBehaviorCallbacks(actor);
    return 0;
}

s32 Mimic_ReleaseResources(struct SceneActorRegistryResetView *actor)
{
    SceneActorEffects_ReleaseResources(actor);
    return 0;
}

static inline void MimicInit_InitializeBody(void *object, u32 *fill)
{
    struct SceneActorRegistryResetView *arg0 = object;
    struct SceneActorChaseGroup *group = &arg0->group;
    struct SceneActorBody *body = &arg0->body;
    struct ContactPosition origin;
    s32 mode, secondLimit, firstLimit, gateDuration, variant;
    struct SceneActorRegistryRoute *rt;
    arg0->flags2CC = 0;
    arg0->flags2D0 = 0;
    arg0->enabled = 0;
    arg0->field2E8 = 8;
    arg0->flags2C0 = MonsterInit_ReadKeywordHalfword('M', 0);
    arg0->field2C2 = 0x40;
    MonsterInit_ClearPosition(fill, &origin);
    mode = Script_GetKeywordValue('N', 0);

    MonsterInit_ReadAttacks(arg0);

    if (Script_SeekToKeyword('l')) {
        firstLimit = MonsterInit_ReadWord(0x12C);
        secondLimit = MonsterInit_ReadWord(0x12C);
        gateDuration = MonsterInit_ReadWord(0x12C);
    } else {
        firstLimit = 0;
        secondLimit = 0;
        gateDuration = 0;
    }
    MonsterInit_InitializeGate(&group->common.gate, gateDuration);
    MonsterInit_ReadRange(&group->common.control);
    MonsterInit_ReadTargeting(&arg0->tracking);
    if (Script_GetKeywordValue('b', 0))
        MonsterInit_SetStatusFlags(arg0, 0x200);
    arg0->field1F0 = MonsterInit_ReadKeywordHalfword('F', 0);
    arg0->field1EC = MonsterInit_ReadKeywordHalfword('R', 0);

    if (Script_SeekToKeyword('A')) {
        MonsterInit_SetInitialHealth(arg0, MonsterInit_ReadHalfword(0));
        group->halfHealth = MonsterInit_Half(MonsterInit_GetInitialHealth(arg0));
        arg0->field9E = MonsterInit_ReadByte(0);
        arg0->field9F = MonsterInit_ReadByte(0);
        arg0->fieldA0 = MonsterInit_ReadByte(0);
        arg0->fieldA2 = MonsterInit_ReadHalfword(0);
        arg0->fieldA4 = MonsterInit_ReadHalfword(0);
    }

    MonsterInit_InitializeHealth(arg0, MonsterInit_GetInitialHealth(arg0), firstLimit, secondLimit);

    if (Script_SeekToKeyword('X')) {
        arg0->fieldAC = MonsterInit_ReadWord(0);
        arg0->fieldA1 = MonsterInit_ReadByte(0);
    }
    arg0->fieldB2 = MonsterInit_ReadKeywordHalfword('K', 2);
    variant = (s16)Script_GetKeywordValue('V', -1);
    if (variant < 0)
        variant = 0xFFFF;
    arg0->field9A = variant;
    if (Script_GetKeywordValue('H', 0))
        MonsterInit_SetStatusFlags(arg0, 0x800);
    if (Script_GetKeywordValue('L', 0))
        MonsterInit_SetStatusFlags(arg0, 0x8000);

    SceneBody_Init(body, mode, &origin, 0, 4, arg0);
    rt = &group->homeRoute;
    if (MonsterInit_ReadRoute(arg0, group, rt)) {
        MonsterInit_SnapToRoute(&arg0->body.position, rt);
        MonsterInit_DrawablePosition(arg0);
        arg0->body.direction = Route_GetCurrentPointFacing(rt);
    }

    if (MonsterInit_TestMotionFlags(arg0, 0x200))
        TargetRegistry_RegisterActor(&group->common.placement, body,
                                     SceneActorReaction_SetMotionMode);
    SceneBody_SetSprite(body, MonsterInit_GetDrawableKind(arg0) == 0 ? arg0->drawable : 0);
    if (MonsterInit_GetCategory(arg0) == 0x15)
        SceneBody_SetMovementProbe(body, ((struct Mimic *)arg0)->movementProbe, 0x3C, 0x3C);
    else
        SceneBody_SetMovementProbe(body, ((struct Mimic *)arg0)->movementProbe, 0x40, 0x40);

    group->common.bodyId = arg0->body.id;
    group->common.bodyPosition = &arg0->body.position;
}

static inline void MimicInit_InstallCompletionCallbacks(struct SceneActorRegistryResetView *arg0)
{
    arg0->messageCallback = SceneActorScript_OnCommand;
    arg0->updateCallback = Mimic_OnUpdate;
    arg0->destroyCallback = Mimic_ReleaseResources;
}

static inline void MimicInit_SetPaletteBank(struct SceneActorRegistryResetView *actor, u16 palette)
{
    actor->paletteBank = palette;
}

static inline void MimicInit_InstallRoutes(struct SceneActorRegistryResetView *actor)
{
    actor->callback248 = Mimic_ResetPursuitDrawable;
    actor->field23C = SceneActor_PrepareStationaryReaction;
}

static inline void MimicInit_InitializeTerrainCell(struct SceneActorRegistryResetView *actor,
                                                   s32 cell)
{
    actor->terrainCell = cell;
    actor->previousTerrainCell = cell;
}

s32 Mimic_Init(void *object)
{
    struct SceneActorRegistryResetView *instance = object;
    struct SceneActorRegistryResetView *actor = object;
    struct SceneActorChaseGroup *group = &instance->group;
    struct SceneActorBody *body = &instance->body;
    u32 zero;
    struct ContactPosition tmp;
    u32 flag;
    s32 lift;

    SceneActorRegistry_SetObjectContext(actor, actor, 0, 1);
    MonsterInit_SetCategory(actor, 10);
    Mimic_InitProfile(actor);
    if (MonsterInit_GetDrawableKind(actor) == 0) {
        u8 *heap;
        actor->drawable = Heap_Alloc(0x5C);
        Memory_Clear(actor->drawable, 0x5C);
        heap = MonsterInit_Drawable(actor);
        MonsterInit_InitializeSprite(heap, 0x3876, 0x9AF2);
        if (MonsterInit_GetCategory(actor) == 0x11 || MonsterInit_GetCategory(actor) == 0x17) {
            u8 *priority = &((struct ActorSprite *)heap)->priority;
            *priority = 1;
        }
        actor->body.direction = 0;
    } else {
        u8 *heap;
        actor->drawable = Heap_Alloc(0x80);
        Memory_Clear(actor->drawable, 0x80);
        heap = MonsterInit_Drawable(actor);
        MonsterInit_ClearPosition(&zero, &tmp);
        MonsterInit_InitializeWidget(heap, 0xCB05, 0x3876, &tmp);
        actor->animationIndex = 0xFF;
    }
    MimicInit_InitializeBody(instance, &zero);
    ((struct Mimic *)instance)->healthThreshold = MonsterInit_GetHealth(actor) >> 1;
    actor->body.direction = Script_GetKeywordValue('d', 0);
    ((struct Mimic *)instance)->leapSpeed = Script_GetKeywordValue('s', 10);
    if (Script_SeekToKeyword('p')) {
        s32 cell;
        actor->body.position.x = MonsterInit_ReadHalfword(0);
        actor->body.position.y = MonsterInit_ReadHalfword(0);
        actor->body.position.z = MonsterInit_ReadHalfword(0);
        MimicInit_InitializeTerrainCell(actor, MonsterInit_CellIndex(&actor->body.position));
        actor->groundHeight2BE =
            MonsterInit_TerrainHeight(MonsterInit_GetTerrainCell(actor), &actor->body.position);
        actor->field2EA = 0;
        actor->body.position.y = actor->groundHeight2BE;
        MonsterInit_DrawablePosition(actor);
    }
    ((struct Mimic *)instance)->origin = actor->body.position;
    ((struct Mimic *)instance)->originCell = actor->terrainCell;
    MonsterInit_ReadPalette(actor, 0x304, 0x192);
    MimicInit_SetPaletteBank(actor, 0x306);
    SceneActor_InitializeReactionColliders(actor);
    MonsterInit_InstallOpaqueContactCallbacks(
        actor, Mimic_UpdateBehavior, 0, 0, SceneActor_EnterFlaggedEntryPhase,
        Mimic_OnAnimationStateChanged, 0, Mimic_ConvergeAndTurn, Mimic_UpdateDefeatBlink,
        SceneActorReaction_OnEnemyDefault, SceneActor_AcquireCollisionTarget,
        SceneActorTarget_ResolvePendingObject, SceneActorReaction_ResolvePlacementDamage,
        Mimic_StartHitReaction);
    MonsterInit_InitializeGroupWithBounds(group, &((struct Mimic *)instance)->routeProbe,
                                          &((struct Mimic *)instance)->destination, 0, 0,
                                          Mimic_OnBehaviorComplete, 0, 1, body);
    MimicInit_InstallRoutes(actor);
    MonsterInit_SetBehaviorFlags(actor, 0x400200);
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
    if (Script_GetKeywordValue('S', 0) != 0 || !MonsterInit_IsInCurrentRegion(actor))
        SceneActorPlacement_HideActor(actor, 0);
    SceneActorAnimation_SelectDirectionalEntry(actor, 0);
    if (MonsterInit_GetDrawableKind(actor) == 0)
        DisplaySequence_Step(actor->drawable);
    else
        Anim_Step(&((struct SceneActorAnimationTileDrawable *)actor->drawable)->tile,
                  actor->drawable);
    MonsterInit_ClearReceiver(actor);
    lift = Script_GetKeywordValue('f', 0);
    if (lift) {
        actor->field2EA = lift;
        MonsterInit_SetControl(actor, SceneActor_RestoreSavedTargetOnCompletion, 0x26);
        MonsterInit_SetState(actor, 3, 0, 0);
    }
    SceneActorBehavior_InstallCallbackTables(actor);
    Mimic_InitBehaviorTable(actor);
    Mimic_InitActionTable(actor);
    actor->field2E8 = 0;
    MonsterInit_SaveControl(MonsterInit_SavedControlOf(actor), actor);
    MimicInit_InstallCompletionCallbacks(actor);
    if (!SceneActorRegistry_LinkActor(actor))
        return -1;
    return 0;
}

void Mimic_Create(void)
{
    u8 *p = Heap_Alloc(0x664);
    if (p != 0) {
        Memory_Clear(p, 0x664);
        if (Mimic_Init(p) < 0) {
            Mimic_ReleaseResources(p);
            Heap_Free(p);
        }
    }
}
