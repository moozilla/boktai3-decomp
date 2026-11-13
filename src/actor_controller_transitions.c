#include "actor_controller.h"

void ActorController_UpdateNegativeTurnSpeed91(struct SceneCombatActor *actor)
{
    Armor_UpdateNegativeTurn(actor, 0x5b);
}

void ActorController_UpdatePositiveTurnSpeed32(struct SceneCombatActor *actor)
{
    Armor_UpdatePositiveTurn(actor, 0x20);
}

void ActorController_UpdateNegativeTurnSpeed32(struct SceneCombatActor *actor)
{
    Armor_UpdateNegativeTurn(actor, 0x20);
}

extern const u32 gUnk_0824EBE4[];

/* Tentative reset-motion transition; the optional argument sets speed to 120. */

void ActorController_UpdateResetMotionTransition(struct SceneCombatActor *actor, s32 setSpeed)
{
    u8 *phase = &actor->actionPhase;
    u8 initialPhase = *phase;

    switch (initialPhase) {
        case 0: {
            u32 four = 4;
            Armor_SetAnimationFromStart(
                actor,
                &gUnk_0824EBE4[((actor->heading + 0x60) >> 6) & 3]);
            (*phase)++;
            actor->actionReady = initialPhase;
            Armor_StartFullPaletteRestoreDelay(actor);
            if (setSpeed != 0)
                actor->animationDuration = 120;
            ActorController_ClearColliderFlags(&actor->colliderGroup.colliders[2], four);
            ActorController_ClearColliderFlags(&actor->colliderGroup.colliders[1], four);
            ActorController_ClearColliderFlags(&actor->colliderGroup.colliders[3], four);
            ActorController_SetColliderFlags(&actor->colliderGroup.colliders[0], four);
            {
                u32 one = 1;
                ActorController_ClearMotionFlags(&actor->colliderGroup.motion, one);
            }
            actor->flags &= ~0x20;
            break;
        }
        case 1:
            if (actor->animationComplete != 0) {
                *phase = 0xFF;
                actor->actionReady = initialPhase;
            }
            break;
    }
}

void ActorController_UpdateResetMotion(struct SceneCombatActor *actor)
{
    ActorController_UpdateResetMotionTransition(actor, 0);
}

void ActorController_UpdateResetMotionSpeed120(struct SceneCombatActor *actor)
{
    ActorController_UpdateResetMotionTransition(actor, 1);
}

extern const u32 gUnk_0824EBF4[];

extern const struct ContactPosition gUnk_0824EC04[], gUnk_0824EC24[], gUnk_0824EC44[], gUnk_0824EC64[];

void Collider_SetAttackPowerAndForce(void *, s32, s32);

static inline s32 ActorController_HalfAttack(s32 value)
{
    value <<= 11;
    return value >= 0 ? value >> 12 : -((-value) >> 12);
}

void ActorController_UpdateAttackCollider(struct SceneCombatActor *actor)
{
    u8 *phase = &actor->actionPhase;
    u8 initialPhase = *phase;

    switch (initialPhase) {
        case 0: {
            u32 four = 4;
            Armor_SetAnimationFromStart(actor, &gUnk_0824EBF4[((actor->heading + 0x60) >> 6) & 3]);
            (*phase)++;
            actor->actionReady = initialPhase;
            ActorController_ClearColliderFlags(&actor->colliderGroup.colliders[2], four);
            ActorController_ClearColliderFlags(&actor->colliderGroup.colliders[1], four);
            ActorController_ClearColliderFlags(&actor->colliderGroup.colliders[3], four);
            ActorController_SetColliderFlags(&actor->colliderGroup.colliders[0], four);
            {
                u32 one = 1;
                ActorController_ClearMotionFlags(&actor->colliderGroup.motion, one);
            }
            actor->flags &= ~0x20;
            break;
        }
        case 1:
            if (actor->animationComplete != 0) {
                *phase = 0xFF;
                actor->actionReady = initialPhase;
                {
                    u16 *flags = &actor->colliderGroup.colliders[0].flags;
                    u32 oldFlags = *flags;
                    u32 activeFlag = 4;
                    activeFlag |= oldFlags;
                    *flags = activeFlag;
                }
            }
            break;
    }
    switch (actor->animationEvent) {
        case 4: {
            struct ContactCollider *collider;
            u8 *facing;
            u32 tableOffset;
            actor->colliderGroup.colliders[0].flags &= ~4;
            {
                struct ContactCollider *attackCollider = &actor->colliderGroup.colliders[0];
                u32 mask = 0x60000;
                attackCollider->reactionFlags |= mask;
            }
            Collider_SetAttackPowerAndForce(&actor->colliderGroup.colliders[0], (u16)ActorController_HalfAttack((s16)actor->attack), 50);
            facing = &actor->heading;
            tableOffset = ((*facing + 0x60) >> 6) & 3;
            collider = &actor->colliderGroup.colliders[0];
            tableOffset *= sizeof(struct ContactPosition);
            collider->bounds = *(const struct ContactPosition *)(tableOffset + (const u8 *)gUnk_0824EC24);
            collider->offset = *(const struct ContactPosition *)(tableOffset + (const u8 *)gUnk_0824EC04);
            {
                u8 heading = *facing;
                actor->colliderGroup.colliders[0].direction = heading;
            }
            break;
        }
        case 5: {
            struct ContactCollider *collider;
            u32 tableOffset = ((actor->heading + 0x60) >> 6) & 3;
            collider = &actor->colliderGroup.colliders[0];
            tableOffset *= sizeof(struct ContactPosition);
            collider->bounds = *(const struct ContactPosition *)(tableOffset + (const u8 *)gUnk_0824EC64);
            collider->offset = *(const struct ContactPosition *)(tableOffset + (const u8 *)gUnk_0824EC44);
            if (actor->animationFrame == 0)
                Sound_PlaySongChecked(0xBF);
            break;
        }
    }
}

extern const u32 gUnk_0824EC84[];

extern const struct ContactPosition gUnk_0824ECB4, gUnk_0824EC94[];

extern const u8 gUnk_0824ECBC[];

void Camera_RequestShake(u32);

void SceneParticleBurst_CreateWithPause(void *, u32, const void *, u32, u32, u32);

static inline s32 ActorController_TruncateFixed(s32 value)
{
    return value >= 0 ? value >> 12 : -((-value) >> 12);
}

void ActorController_UpdateMovingAttackCollider(struct SceneCombatActor *actor)
{
    u8 *phase = &actor->actionPhase;
    u8 initialPhase = *phase;
    switch (initialPhase) {
        case 0: {
            u32 four = 4;
            Armor_SetAnimationFromStart(actor, &gUnk_0824EC84[((actor->heading + 0x60) >> 6) & 3]);
            (*phase)++;
            actor->actionReady = initialPhase;
            Sound_PlaySongChecked(0xb9);
            ActorController_ClearColliderFlags(&actor->colliderGroup.colliders[2], four);
            ActorController_ClearColliderFlags(&actor->colliderGroup.colliders[1], four);
            ActorController_ClearColliderFlags(&actor->colliderGroup.colliders[3], four);
            ActorController_SetColliderFlags(&actor->colliderGroup.colliders[0], four);
            {
                u32 one = 1;
                ActorController_ClearMotionFlags(&actor->colliderGroup.motion, one);
            }
            actor->flags &= ~0x20;
            break;
        }
        case 1:
            if (actor->animationComplete) {
                *phase = 255;
                actor->actionReady = initialPhase;
            }
            break;
    }
    switch (actor->animationEvent) {
        case 9: {
            struct ContactCollider *collider;
            const struct ContactPosition *offset;
            union ActorControllerPosition out;
            u8 *facing;
            u32 tableOffset, zero;
            actor->shakeStrength = 0x3f;
            actor->colliderGroup.colliders[0].flags &= ~4;
            {
                struct ContactCollider *attackCollider = &actor->colliderGroup.colliders[0];
                u32 mask = 0x60000;
                attackCollider->reactionFlags |= mask;
            }
            Collider_SetAttackPowerAndForce(&actor->colliderGroup.colliders[0], (u16)ActorController_TruncateFixed((s16)actor->attack << 12), 100);
            collider = &actor->colliderGroup.colliders[0];
            collider->bounds = gUnk_0824ECB4;
            facing = &actor->heading;
            tableOffset = ((*facing + 0x60) >> 6) & 3;
            tableOffset *= sizeof(struct ContactPosition);
            collider->offset = *(const struct ContactPosition *)(tableOffset + (const u8 *)gUnk_0824EC94);
            {
                u8 heading = *facing;
                u8 *dest = &actor->colliderGroup.colliders[0].direction;
                zero = 0;
                *dest = heading;
            }
            offset = (const void *)(tableOffset + (const u8 *)gUnk_0824EC94);
            {
                s16 *dest = out.components;
                const struct ContactPosition *left = offset;
                const struct ContactPosition *right = &actor->navigation.referencePosition;
                dest[0] = right->x + left->x;
                dest[1] = right->y + left->y;
                dest[2] = right->z + left->z;
            }
            SceneParticleBurst_CreateWithPause(&out, 0, gUnk_0824ECBC, 15, 1, zero);
            if (actor->animationFrame == 0)
                Sound_PlaySongChecked(0xba);
            break;
        }
        case 11: {
            struct ContactCollider *attackCollider = &actor->colliderGroup.colliders[0];
            u32 mask = 0xfff9ffff;
            attackCollider->reactionFlags &= mask;
        }
            Collider_SetAttackPowerAndForce(&actor->colliderGroup.colliders[0], (u16)ActorController_TruncateFixed((s16)actor->attack << 11), 50);
            break;
        case 14: {
            u16 *flags = &actor->colliderGroup.colliders[0].flags;
            u32 value = *flags;
            u32 four = 4;
            four |= value;
            *flags = four;
            break;
        }
    }
    {
        u8 *strength = &actor->shakeStrength;
        if (*strength) {
            Camera_RequestShake(*strength);
            (*strength)--;
        }
    }
}

void ActorController_UpdateMovementVelocity(struct SceneCombatActor *actor)
{
    ActorController_UpdateMovingAttackCollider(actor);
    if ((u16)(actor->animationEvent - 7) <= 4) {
        s16 *velocity;
        u8 *heading;
        int angle, speed;
        actor->flags |= 0x20;
        heading = &actor->heading;
        velocity = actor->velocity;
        angle = *heading;
        speed = 25;
        {
            int x = ActorController_TruncateFixed(gUnk_086149C4[(angle + 64) & 255] * speed);
            int zero = 0;
            velocity[0] = x;
            velocity[1] = zero;
        }
        velocity[2] = ActorController_TruncateFixed(gUnk_086149C4[angle] * speed);
    } else
        actor->flags &= ~0x20;
}

void ActorController_UpdateDecayingVelocity(struct SceneCombatActor *actor)
{
    ActorController_UpdateMovingAttackCollider(actor);
    {
        u8 *heading = &actor->heading;
        int shift = actor->actionCounter >> 1;
        int speed = 20;
        s16 *velocity = actor->velocity;
        int angle = *heading;
        speed >>= shift;
        {
            int x = ActorController_TruncateFixed(gUnk_086149C4[(angle + 64) & 255] * speed);
            int zero = 0;
            velocity[0] = x;
            velocity[1] = zero;
        }
        {
            const s16 *table = gUnk_086149C4;
            const s16 *entry = (const void *)((angle * sizeof(*table)) + (const u8 *)table);
            velocity[2] = ActorController_TruncateFixed(*entry * speed);
        }
    }
    if (actor->velocity[0] || actor->velocity[2]) {
        u16 *flags = &actor->flags;
        u32 value = *flags;
        u32 mask = 0x20;
        value |= mask;
        *flags = value;
        return;
    }
    actor->flags &= ~0x20;
}

extern const u32 gUnk_0824ECC4[], gUnk_0824ECC8[];

extern const struct ContactPosition gUnk_0824ED0C[], gUnk_0824ECCC[];

extern const u8 gUnk_0824ED4C[];

extern const struct ContactPosition gUnk_0824EC04[];

extern const struct ContactPosition gUnk_0824EC24[];

extern const struct ContactPosition gUnk_0824EC44[];

extern const struct ContactPosition gUnk_0824EC64[];

void Armor_StartFastWhiteFade(struct SceneCombatActor *);

void Armor_StartPaletteTransition(struct SceneCombatActor *);

static inline s32 ActorController_ScaleHeavyAttack(s32 attack)
{
    s32 scaledAttack = 0xaaa * attack;
    return scaledAttack >= 0 ? scaledAttack >> 12 : -((-scaledAttack) >> 12);
}

void ActorController_UpdateHeavyAttackCollider(struct SceneCombatActor *actor)
{
    u8 *phase = &actor->actionPhase;
    s32 initialPhase = *phase;
    switch (initialPhase) {
        case 0: {
            u32 four = 4;
            Armor_SetAnimationFromStart(actor, gUnk_0824ECC4);
            (*phase)++;
            actor->actionReady = initialPhase;
            ActorController_SetColliderFlags(&actor->colliderGroup.colliders[2], four);
            ActorController_ClearColliderFlags(&actor->colliderGroup.colliders[1], four);
            ActorController_ClearColliderFlags(&actor->colliderGroup.colliders[3], four);
            ActorController_SetColliderFlags(&actor->colliderGroup.colliders[0], four);
            {
                struct ActorControllerMotion *motion = &actor->colliderGroup.motion;
                u32 one = 1;
                motion->flags |= one;
            }
            actor->flags &= ~0x20;
            break;
        }
        case 1:
            if (actor->animationComplete) {
                u32 four = 4;
                struct ContactCollider *collider;
                Armor_SetAnimationFromStart(actor, gUnk_0824ECC8);
                (*phase)++;
                actor->actionReady = initialPhase;
                ActorController_SetColliderFlags(&actor->colliderGroup.colliders[2], four);
                ActorController_ClearColliderFlags(&actor->colliderGroup.colliders[1], four);
                ActorController_ClearColliderFlags(&actor->colliderGroup.colliders[3], four);
                ActorController_ClearColliderFlags(&actor->colliderGroup.colliders[0], four);
                {
                    struct ActorControllerMotion *motion = &actor->colliderGroup.motion;
                    u32 one = 1;
                    motion->flags |= one;
                }
                actor->flags &= ~0x20;
                {
                    struct ActorControllerColliderGroup *group = &actor->colliderGroup;
                    struct ContactCollider *attackCollider;
                    u32 mask;
                    attackCollider = &group->colliders[0];
                    mask = 0x60000;
                    attackCollider->reactionFlags |= mask;
                    collider = attackCollider;
                }
                Collider_SetAttackPowerAndForce(collider, (u16)ActorController_ScaleHeavyAttack((s16)actor->attack), 80);
            }
            break;
        case 2:
            if (actor->animationStatus & 1)
                actor->actionReady = 1;
            else
                actor->actionReady = 0;
            break;
    }
    if (actor->actionPhase) {
        u32 frame = actor->animationEvent;
        struct ContactCollider *collider = &actor->colliderGroup.colliders[0];
        u32 tableOffset = frame * sizeof(struct ContactPosition);
        collider->bounds = *(const struct ContactPosition *)(tableOffset + (const u8 *)gUnk_0824ED0C);
        collider->offset = *(const struct ContactPosition *)(tableOffset + (const u8 *)gUnk_0824ECCC);
        {
            u8 heading = gUnk_0824ED4C[frame];
            actor->colliderGroup.colliders[0].direction = heading;
        }
        if (frame == 3 && actor->animationFrame == 0)
            Sound_PlaySongChecked(0xbe);
    }
}

extern const u32 gUnk_0824ED54[];

void ActorController_UpdateFixedAnimationTransition(struct SceneCombatActor *actor)
{
    u8 *phase = &actor->actionPhase;
    u8 initialPhase = *phase;
    switch (initialPhase) {
        case 0: {
            u32 four = 4;
            Armor_SetAnimationFromStart(actor, gUnk_0824ED54);
            (*phase)++;
            actor->actionReady = initialPhase;
            ActorController_SetColliderFlags(&actor->colliderGroup.colliders[2], four);
            ActorController_ClearColliderFlags(&actor->colliderGroup.colliders[1], four);
            ActorController_ClearColliderFlags(&actor->colliderGroup.colliders[3], four);
            ActorController_SetColliderFlags(&actor->colliderGroup.colliders[0], four);
            {
                struct ActorControllerMotion *render = &actor->colliderGroup.motion;
                u32 one = 1;
                render->flags |= one;
            }
            actor->flags &= ~0x20;
            break;
        }
        case 1:
            if (actor->animationComplete) {
                *phase = 0xff;
                actor->actionReady = initialPhase;
                actor->heading = 0x40;
            }
            break;
    }
}

extern u32 gUnk_0824ED58[];

extern u32 gUnk_0824ED68[];

void ActorController_UpdateFadeAnimationTransition(struct SceneCombatActor *actor)
{
    u8 *phase = &actor->actionPhase;
    u8 initialPhase = *phase;

    switch (initialPhase) {
        case 0: {
            u32 four = 4;
            Armor_SetAnimationFromStart(
                actor, &gUnk_0824ED58[((actor->heading + 0x60) >> 6) & 3]);
            (*phase)++;
            actor->actionReady = initialPhase;
            ActorController_ClearColliderFlags(&actor->colliderGroup.colliders[2], four);
            ActorController_ClearColliderFlags(&actor->colliderGroup.colliders[1], four);
            ActorController_ClearColliderFlags(&actor->colliderGroup.colliders[3], four);
            ActorController_SetColliderFlags(&actor->colliderGroup.colliders[0], four);
            {
                u32 one = 1;
                ActorController_ClearMotionFlags(&actor->colliderGroup.motion, one);
            }
            actor->flags &= ~0x20;
            Sound_PlaySongChecked(0xC1);
            break;
        }
        case 1:
            if (actor->animationComplete != 0) {
                Armor_SetAnimationFromStart(
                    actor, &gUnk_0824ED68[((actor->heading + 0x60) >> 6) & 3]);
                {
                    u32 next = *phase + 1;
                    u32 zero = 0;
                    *phase = next;
                    actor->actionCounter = zero;
                }
                Armor_StartFastWhiteFade(actor);
            } else
                break;
        case 2:
            if (actor->actionCounter > 7) {
                actor->actionPhase = 0xFF;
                actor->actionReady = 1;
            }
            break;
    }
}

extern u32 gUnk_0824ED78[];

extern u32 gUnk_0824ED88[];

void ActorController_UpdateSoundedAnimationTransition(struct SceneCombatActor *actor)
{
    u8 *phase = &actor->actionPhase;
    u8 initialPhase = *phase;

    switch (initialPhase) {
        case 0: {
            u32 four = 4;
            Armor_SetAnimationFromStart(actor, &gUnk_0824ED78[((actor->heading + 0x60) >> 6) & 3]);
            (*phase)++;
            actor->actionReady = initialPhase;
            ActorController_ClearColliderFlags(&actor->colliderGroup.colliders[2], four);
            ActorController_ClearColliderFlags(&actor->colliderGroup.colliders[1], four);
            ActorController_ClearColliderFlags(&actor->colliderGroup.colliders[3], four);
            ActorController_SetColliderFlags(&actor->colliderGroup.colliders[0], four);
            {
                u32 one = 1;
                ActorController_ClearMotionFlags(&actor->colliderGroup.motion, one);
            }
            actor->flags &= ~0x20;
            Sound_PlaySongChecked(0xC1);
            break;
        }
        case 1:
            if (actor->animationComplete != 0) {
                Armor_SetAnimationFromStart(actor,
                                            &gUnk_0824ED88[((actor->heading + 0x60) >> 6) & 3]);
                *phase = 0xFF;
                actor->actionReady = initialPhase;
            }
            break;
    }
}

extern u32 gUnk_0824ED98[];

void ActorController_UpdatePaletteAnimationTransition(struct SceneCombatActor *actor)
{
    u8 *phase = &actor->actionPhase;
    u8 initialPhase = *phase;

    switch (initialPhase) {
        case 0: {
            u32 four = 4;
            Armor_SetAnimationFromStart(
                actor, &gUnk_0824ED98[((actor->heading + 0x60) >> 6) & 3]);
            (*phase)++;
            actor->actionReady = initialPhase;
            ActorController_ClearColliderFlags(&actor->colliderGroup.colliders[2], four);
            ActorController_ClearColliderFlags(&actor->colliderGroup.colliders[1], four);
            ActorController_ClearColliderFlags(&actor->colliderGroup.colliders[3], four);
            ActorController_SetColliderFlags(&actor->colliderGroup.colliders[0], four);
            {
                u32 one = 1;
                ActorController_ClearMotionFlags(&actor->colliderGroup.motion, one);
            }
            actor->flags &= ~0x20;
            Armor_StartPaletteTransition(actor);
            break;
        }
        case 1:
            if (actor->animationComplete != 0) {
                *phase = 0xFF;
                actor->actionReady = initialPhase;
            }
            break;
    }
}

extern const u32 gUnk_0824EDA8[];

extern const u32 gUnk_0824EDB8[];

void ActorController_UpdateLayeredAnimationTransition(struct SceneCombatActor *actor)
{
    u8 *phase = &actor->actionPhase;
    u8 initialPhase = *phase;
    switch (initialPhase) {
        case 0: {
            u32 four = 4;
            Armor_SetAnimationFromStart(
                actor, &gUnk_0824EDA8[((actor->heading + 0x60) >> 6) & 3]);
            (*phase)++;
            actor->actionReady = initialPhase;
            Sound_PlaySongChecked(0xc1);
            ActorController_SetColliderFlags(&actor->colliderGroup.colliders[2], four);
            ActorController_ClearColliderFlags(&actor->colliderGroup.colliders[1], four);
            ActorController_ClearColliderFlags(&actor->colliderGroup.colliders[3], four);
            ActorController_SetColliderFlags(&actor->colliderGroup.colliders[0], four);
            {
                struct ActorControllerMotion *render = &actor->colliderGroup.motion;
                u32 one = 1;
                render->flags |= one;
            }
            actor->flags &= ~0x20;
            break;
        }
        case 1:
            if (actor->animationComplete) {
                Armor_SetAnimationFromStart(
                    actor, &gUnk_0824EDB8[((actor->heading + 0x60) >> 6) & 3]);
                *phase = 0xff;
                actor->actionReady = initialPhase;
            }
            break;
    }
}

extern const u32 gUnk_0824EDC8[];

void DirectionalActor_UpdateSequence(struct SceneCombatActor *actor)
{
    u8 *phase = &actor->actionPhase;
    u8 initialPhase = *phase;
    switch (initialPhase) {
        case 0:
            Armor_SetAnimationFromStart(actor, &gUnk_0824EDC8[((actor->heading + 0x60) >> 6) & 3]);
            (*phase)++;
            actor->actionReady = initialPhase;
            Armor_StartPaletteTransition(actor);
            break;
        case 1:
            if (actor->animationComplete != 0) {
                *phase = 0xff;
                actor->actionReady = initialPhase;
            }
            break;
    }
}
