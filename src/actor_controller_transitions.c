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

static inline void ActorController_ClearColliderFlags(struct ContactCollider *collider, u32 flags)
{
    collider->flags &= ~flags;
}

static inline void ActorController_SetColliderFlags(struct ContactCollider *collider, u32 flags)
{
    collider->flags |= flags;
}

static inline void ActorController_ClearMotionFlags(struct ActorControllerMotion *motion, u32 flags)
{
    motion->flags &= ~flags;
}

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
            ActorController_ClearColliderFlags(&actor->colliders[2], four);
            ActorController_ClearColliderFlags(&actor->colliders[1], four);
            ActorController_ClearColliderFlags(&actor->colliders[3], four);
            ActorController_SetColliderFlags(&actor->colliders[0], four);
            {
                u32 one = 1;
                ActorController_ClearMotionFlags(&actor->motion, one);
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
            ActorController_ClearColliderFlags(&actor->colliders[2], four);
            ActorController_ClearColliderFlags(&actor->colliders[1], four);
            ActorController_ClearColliderFlags(&actor->colliders[3], four);
            ActorController_SetColliderFlags(&actor->colliders[0], four);
            {
                u32 one = 1;
                ActorController_ClearMotionFlags(&actor->motion, one);
            }
            actor->flags &= ~0x20;
            break;
        }
        case 1:
            if (actor->animationComplete != 0) {
                *phase = 0xFF;
                actor->actionReady = initialPhase;
                {
                    u16 *flags = &actor->colliders[0].flags;
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
            actor->colliders[0].flags &= ~4;
            {
                struct ContactCollider *attackCollider = &actor->colliders[0];
                u32 mask = 0x60000;
                attackCollider->reactionFlags |= mask;
            }
            Collider_SetAttackPowerAndForce(&actor->colliders[0], (u16)ActorController_HalfAttack((s16)actor->attack), 50);
            facing = &actor->heading;
            tableOffset = ((*facing + 0x60) >> 6) & 3;
            collider = &actor->colliders[0];
            tableOffset *= sizeof(struct ContactPosition);
            collider->bounds = *(const struct ContactPosition *)(tableOffset + (const u8 *)gUnk_0824EC24);
            collider->offset = *(const struct ContactPosition *)(tableOffset + (const u8 *)gUnk_0824EC04);
            {
                u8 heading = *facing;
                actor->colliders[0].direction = heading;
            }
            break;
        }
        case 5: {
            struct ContactCollider *collider;
            u32 tableOffset = ((actor->heading + 0x60) >> 6) & 3;
            collider = &actor->colliders[0];
            tableOffset *= sizeof(struct ContactPosition);
            collider->bounds = *(const struct ContactPosition *)(tableOffset + (const u8 *)gUnk_0824EC64);
            collider->offset = *(const struct ContactPosition *)(tableOffset + (const u8 *)gUnk_0824EC44);
            if (actor->animationFrame == 0)
                Sound_PlaySongChecked(0xBF);
            break;
        }
    }
}
