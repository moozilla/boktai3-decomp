#include "pooled_actor_controller.h"

void Actor_InitControllerFromScript(struct PooledActor *actor)
{
    struct ActorScriptPosition position;
    s32 scriptId = Script_GetKeywordValue(0x6e, 0);
    s32 health;
    if (Script_SeekToKeyword(0x70)) {
        position.x = Script_GetValue();
        position.y = Script_GetValue();
        position.z = Script_GetValue();
    } else {
        position.x = 0;
        position.y = 0;
        position.z = 0;
    }
    Actor_InitEntity(actor, scriptId, Script_GetKeywordValue(0x64, 0), (void *)&position);
    actor->flags = 0;
    actor->frameFlags = 0;
    actor->pendingDamage = 0;
    actor->damageSource = 0;
    actor->attackFlags = 0;
    actor->targetRangeSquared = Script_GetKeywordValue(0x65, 0x10000);
    {
        struct PooledActor *q = actor;
        q->targetArc = Script_GetKeywordValue(0x45, 0x80);
        q->targetDistanceSquared = -1;
        q->targetAngleDelta = 0x7f;
        q->targetIndex = 0xff;
    }
    actor->targetCount = Script_GetKeywordValue(0x73, 1);
    health = Script_GetKeywordValue(0x6c, 0x64);
    Actor_SetHealthLimits(actor, health, health);
    if (Script_SeekToKeyword(0x53)) {
        actor->bodyProperty = Script_GetValue();
        actor->attackProperty0 = Script_GetValue();
        actor->attackProperty1 = Script_GetValue();
        actor->attackProperty2 = Script_GetValue();
        actor->attackProperty3 = Script_GetValue();
        actor->attackProperty4 = Script_GetValue();
        actor->bodyProperty1 = Script_GetValue();
        actor->bodyProperty2 = Script_GetValue();
    } else {
        actor->bodyProperty = 10;
        actor->attackProperty0 = 50;
        actor->attackProperty1 = 20;
        actor->attackProperty2 = 10;
        actor->attackProperty3 = 0;
        actor->attackProperty4 = 0;
        actor->bodyProperty1 = 0;
        actor->bodyProperty2 = 0;
    }
    actor->action = Script_GetKeywordValue(0x44, 0);
    if (Script_SeekToKeyword(0x61)) {
        u32 *q = actor->actionParameters;
        s32 i = 3;
        do {
            *q++ = Script_GetValue();
        } while (--i >= 0);
    }
}

void Actor_InitializeSequenceDrawable(struct PooledActor *actor, s32 resourceId, s32 sequenceId,
                                      s32 flags)
{
    struct DisplayAnimation *drawable = &actor->drawable;
    DisplayAnimation_InitializeSequence(drawable, resourceId, sequenceId, flags,
                                        (const struct DisplayPosition *)&actor->body.position);
    actor->paletteBank = DisplayAnimation_GetTileIndex(drawable);
}

void Actor_InitializeKeyframeDrawable(struct PooledActor *actor, s32 resourceId, s32 flags)
{
    DisplayAnimation_InitializeKeyframes(&actor->drawable, resourceId, flags,
                                         (const struct DisplayPosition *)&actor->body.position);
}

void Actor_UpdateDrawable(struct PooledActor *actor)
{
    void *drawable;
    Actor_RefreshTarget(actor);
    drawable = &actor->drawable;
    DisplayAnimation_SetPosition(drawable, (const struct DisplayPosition *)&actor->body.position);
    {
        u32 mask = 2;
        if (actor->flags & mask)
            DisplayAnimation_UpdatePlayback(drawable);
    }
}

void Actor_PrepareStateUpdate(struct PooledActor *actor)
{
}

void Actor_DispatchActionCallback(struct PooledActor *actor)
{
    int tick;
    actor->frameFlags = 0;
    tick = actor->actionTicks;
    actor->actionTicks = tick + 1;
    if (actor->actionCallback)
        actor->actionCallback(actor, tick);
}

static inline void ActorController_DisableBodyContact(struct PooledActor *actor)
{
    struct ContactCollider *c = &actor->bodyCollider;
    u16 old = c->flags;
    u32 mask = 4;
    mask |= old;
    c->flags = mask;
}

static inline void ActorController_EnableBodyContact(struct PooledActor *actor)
{
    struct ContactCollider *c = &actor->bodyCollider;
    int mask = ~4;
    c->flags &= mask;
}

void Actor_UpdateCollisionAndEffectTimers(struct PooledActor *actor)
{
    struct ActorPoolManager *manager = ActorPool_GetManager();
    int selected = 0;
    if (manager->updateSlot == actor->updateSlot)
        selected = 1;
    if (selected) {
        u32 mask = 4;
        if (actor->flags & mask)
            Collider_Refresh(&actor->hitSensor);
    }
    if (!actor->reaction) {
        u32 mask = 8;
        if (actor->flags & mask)
            ActorController_EnableBodyContact(actor);
        else
            ActorController_DisableBodyContact(actor);
    } else {
        ActorController_DisableBodyContact(actor);
        actor->reaction--;
    }
    if (actor->effectTimer) {
        actor->effectTimer--;
        if (!actor->effectTimer)
            DisplayAnimation_SelectPaletteBank(&actor->drawable, actor->paletteBank);
    }
    Actor_ClearPendingHit(actor);
}

void Actor_RequestAction(struct PooledActor *actor, u16 action)
{
    actor->pendingAction = action;
}

void Actor_ShowDamageNumber(struct PooledActor *actor, u32 amount, u32 kind, void *size)
{
    DamageNumberDisplay_Show(&actor->bodyCollider, amount, kind, &actor->body.position, size, 1);
}

void Actor_StartDamageFlash(struct PooledActor *actor)
{
    DisplayAnimation_SelectPaletteBank(&actor->drawable, 0x192);
    actor->effectTimer = 10;
}

s32 Actor_SetAnimationEntry(struct PooledActor *actor, u32 entry, u32 mode, u32 unused,
                            u32 sequenceFlags, u32 keyframeFlags)
{
    return DisplayAnimation_SetEntry(&actor->drawable, entry, 0, 0, mode, sequenceFlags,
                                     keyframeFlags);
}

int Actor_SetFacingAnimationEntry(struct PooledActor *actor, int entry, int mode, int stride,
                                  int sequenceFlags, int keyframeFlags)
{
    int direction, flip;
    if (actor->drawable.kind == 1) {
        if (actor->facing8 > 4) {
            direction = 8 - actor->facing8;
            flip = 1;
        } else {
            direction = actor->facing8;
            flip = 0;
        }
    } else {
        if (actor->facing8 > 4) {
            direction = 8 - actor->facing8;
            flip = 1;
        } else {
            direction = actor->facing8;
            flip = 0;
        }
        direction *= stride;
    }
    return DisplayAnimation_SetEntry(&actor->drawable, entry, direction, flip, mode, sequenceFlags,
                                     keyframeFlags);
}

s32 Actor_SetFourDirectionAnimationEntry(struct PooledActor *actor, u32 entry, u32 mode, u32 stride,
                                         u32 sequenceFlags, u32 keyframeFlags)
{
    u32 direction, flip;
    struct DisplayAnimation *drawable = &actor->drawable;
    if (drawable->kind == 1) {
        if (actor->facing4 > 1) {
            direction = 3 - actor->facing4;
            flip = 1;
        } else {
            direction = actor->facing4;
            flip = 0;
        }
    } else {
        u8 facing = actor->facing4;
        u8 side = facing - 1;
        direction = 0;
        if (side <= 1)
            direction = stride;
        flip = 0;
        if (facing > 1)
            flip = 1;
    }
    return DisplayAnimation_SetEntry(drawable, entry, direction, flip, mode, sequenceFlags,
                                     keyframeFlags);
}

s32 Actor_SetMirroredAnimationEntry(struct PooledActor *actor, u32 entry, u32 mode, u32 unused,
                                    u32 sequenceFlags, u32 keyframeFlags)
{
    u32 direction, flip;
    struct DisplayAnimation *drawable = &actor->drawable;
    if (drawable->kind == 1) {
        direction = actor->facing4;
        flip = 0;
        if (direction > 1)
            flip = 1;
    } else {
        u8 facing;
        direction = 0;
        facing = actor->facing4;
        flip = 0;
        if (facing > 1)
            flip = 1;
    }
    return DisplayAnimation_SetEntry(drawable, entry, direction, flip, mode, sequenceFlags,
                                     keyframeFlags);
}

void Actor_OnHitboxContact(void *self, void *other, void *actor)
{
    Actor_CapturePendingHit(actor, self, other);
}

void Actor_OnColliderContact(void)
{
}

void Actor_ClearStateChanged(struct PooledActor *actor)
{
    if (actor->stateChanged)
        actor->stateChanged = 0;
}

void Actor_DispatchStateHandler(struct PooledActor *actor)
{
    int tick;
    Actor_PrepareStateUpdate(actor);
    tick = actor->stateTicks;
    actor->stateTicks = tick + 1;
    gUnk_0860504C[actor->state](actor, tick);
}

void Actor_OnTrackingAction(struct PooledActor *actor, u32 tick)
{
    struct MotionPoolParams params;
    struct ActorParticlePosition position;
    s32 (*callback)(void *);
    if (actor->actionChanged)
        actor->actionChanged = 0;
    callback = actor->handleDamage;
    if (callback && callback(actor))
        return;
    callback = actor->selectAction;
    if (callback && callback(actor))
        return;
    Actor_StepFacingAngle(actor);
    Actor_SetFacingAnimationEntry(actor, 1, 0, 1, 0, 0);
    if ((tick & 3) == 0) {
        gRandomIndex = (gRandomIndex + 1) & 0x3ff;
        {
            s32 random = *(u8 *)((gRandomIndex << 1) + (u32)gRandomTable);
            if (random <= 63) {
                MotionPool_InitializeParams(&params);
                MotionPool_SetLifetimeAndVelocity(&params, 0, 0xf0, actor->body.direction + 0x80,
                                                  12);
                MotionPool_SetTracking(
                    &params, 0xb4,
                    ((struct SceneActorBody *)&gPlayers[0]->positionalEffectGroup)->id, 1);
                MotionPool_SetContact(&params, 0, 0x200004, 0x64, 0x32, 0x28);
                MotionPool_SetBounds(&params, 0x10, 0x10, 0x10);
                MotionPool_SetGraphics(&params, 0x61f9, 0x29, 0x29f);
                position = *(struct ActorParticlePosition *)&actor->body.position;
                position.y += 0x80;
                MotionPool_Spawn(&params, &position);
            }
        }
    }
}

s32 Actor_TryStartPendingAction(struct PooledActor *actor)
{
    s32 started;
    if (actor->pendingAction) {
        void (*const *table)(void *, int) = gUnk_08605050;
        u16 id = actor->pendingAction;
        void (*action)(void *, int);
        action = table[id];
        actor->actionCallback = action;
        actor->currentAction = id;
        actor->actionChanged = 1;
        actor->actionTicks = 0;
        started = 1;
    } else
        started = 0;
    return started;
}

int Actor_OnDamage(struct PooledActor *actor)
{
    int damage = actor->pendingDamage;
    if (damage > 0) {
        struct ContactPosition size;
        size.x = 16;
        size.y = 0;
        size.z = 16;
        Actor_ShowDamageNumber(actor, damage, 0, &size);
        Actor_SubtractHealth(actor, damage);
        if (actor->health <= 0) {
            TransientSpriteEffects_Spawn(0, 0, &actor->body.position, 0, 256, 256);
            return 1;
        }
        Actor_StartDamageFlash(actor);
    }
    return 0;
}

void Actor_UpdateAction(struct PooledActor *actor)
{
    Actor_DispatchActionCallback(actor);
}

void Actor_UpdateDrawableAndTarget(void *actor)
{
    Actor_UpdateDrawable(actor);
    Actor_UpdateSelectedTarget(actor);
}

void Actor_UpdateControllerCollisionAndTimers(struct PooledActor *actor)
{
    Actor_UpdateCollisionAndEffectTimers(actor);
}

void Actor_OnControllerUpdate(void *actor)
{
    Actor_UpdateDrawableAndTarget(actor);
    Actor_DispatchStateHandler(actor);
    Actor_UpdateAction(actor);
    Actor_UpdateControllerCollisionAndTimers(actor);
}

void Actor_OnControllerDestroy(struct PooledActor *actor)
{
    DisplayAnimation_Destroy(&actor->drawable);
}

static inline void ActorController_SetSelectionCallback(struct PooledActor *actor, void *fn)
{
    actor->selectAction = fn;
}

static inline void ActorController_SetDamageCallback(struct PooledActor *actor, void *fn)
{
    actor->handleDamage = fn;
}

static inline void ActorController_SetAction(struct PooledActor *actor, void *fn, int zero, int one)
{
    actor->actionCallback = fn;
    actor->currentAction = zero;
    actor->actionChanged = one;
    actor->actionTicks = zero;
}

static inline void ActorController_InitHitSensor(struct PooledActor *actor, struct SceneVec *size,
                                                 void *fn, struct SceneVec *offset)
{
    Actor_InitHitSensor(actor, size, offset, fn);
}

static inline void ActorController_InitBodyCollider(struct PooledActor *actor,
                                                    struct SceneVec *size, void *fn,
                                                    struct SceneVec *offset)
{
    Actor_InitBodyCollider(actor, size, offset, fn);
}

int Actor_InitializeControllerFromScript(struct PooledActor *actor)
{
    struct SceneVec size, offset, size2, offset2;
    int zero;
    Actor_InitControllerFromScript(actor);
    DisplayAnimation_InitializeSequence(&actor->drawable, 0xd495, 0xaf44, 0,
                                        (const struct DisplayPosition *)&actor->body.position);
    {
        int initialZero = 0;
        zero = initialZero;
        Actor_SetFacingAnimationEntry(actor, 1, initialZero, 1, initialZero, initialZero);
    }
    {
        u32 mask = 2;
        actor->flags |= mask;
    }
    size.x = 32;
    size.y = 128;
    size.z = 32;
    offset.x = 0;
    offset.y = 128;
    offset.z = 0;
    ActorController_InitHitSensor(actor, &size, Actor_OnColliderContact, &offset);
    size2.x = 32;
    size2.y = 128;
    size2.z = 32;
    offset2.x = 0;
    offset2.y = 128;
    offset2.z = 0;
    ActorController_InitBodyCollider(actor, &size2, Actor_OnHitboxContact, &offset2);
    ActorController_SetSelectionCallback(actor, Actor_TryStartPendingAction);
    ActorController_SetDamageCallback(actor, Actor_OnDamage);
    actor->state = zero;
    actor->stateChanged = 1;
    actor->stateTicks = zero;
    ActorController_SetAction(actor, Actor_OnTrackingAction, zero, 1);
    return 0;
}

void *Actor_CreateControllerFromScript(void)
{
    struct PooledActor *actor = ActorPool_AllocObject(sizeof(*actor));
    struct PooledActor *allocated = actor;
    if (!actor)
        return 0;
    Actor_SetLifecycleCallbacks(actor, Actor_OnControllerUpdate, Actor_OnControllerDestroy);
    if (Actor_InitializeControllerFromScript(actor) < 0) {
        Actor_RequestRemoval(allocated);
        return 0;
    }
    return actor;
}
