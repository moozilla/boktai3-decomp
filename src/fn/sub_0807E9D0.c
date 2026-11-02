#include "debug.h"
#include "scene_actor_part_burst.h"
#include "scene_actor_behavior.h"
#include "scene_actor_placement.h"
#include "game_state.h"
#include "scene_actor_chase_types.h"
#include "player.h"
s32 Math_Atan2(s32, s32);
s32 Div(s32, s32);
void sub_08220204(void *, void *);
void Collision_CalculateDamageResponse(void *, void *);
void SceneActor_SetContactReactionDirection(void *, void *, void *);
void PlayerStats_AddContactHundredths(void *, void *, s32);

static inline u8 Test(u32 value, u32 mask)
{
    if (value & mask)
        return 1;
    return 0;
}
static inline u8 TestControl(struct SceneActorRegistryResetView *actor, u32 mask)
{
    if (actor->flags2C0 & mask)
        return 1;
    return 0;
}
static inline void MarkContact(struct SceneActorRegistryResetView *actor, u32 mask)
{
    actor->flags2CC = mask | actor->flags2CC;
}
static inline u32 ReadReactionFlags(u32 inputMask, struct SceneActorBehaviorContactView *source)
{
    u32 mask = inputMask;
    return source->reactionFlags & mask;
}
static inline u32 TestMotion(u16 *flags, u32 mask)
{
    return *flags & mask;
}
static inline void AimAtContactTarget(struct SceneActorRegistryResetView *actor)
{
    struct SceneActorChaseTarget *target;
    if (TestControl(actor, 2))
        actor->tracking.target = &((struct SceneActorChasePlayer *)gPlayers[0])->target;
    else if (actor->tracking.target == 0)
        return;
    target = actor->tracking.target;
    {
        s32 dx = target->position.x - actor->body.position.x;
        s32 dy = target->position.z - actor->body.position.z;
        actor->tracking.heading = Math_Atan2(dx, dy);
        actor->tracking.distance = dx * dx + dy * dy;
    }
}
static inline void SelectContactTarget(struct SceneActorBehaviorContactView *source, void *object)
{
    struct SceneActorRegistryResetView *actor = object;
    s32 flagged = 0;
    u16 owners = source->kind;
    if (owners & 1) {
        actor->tracking.target = &((struct SceneActorChasePlayer *)gPlayers[0])->target;
        flagged = 1;
    } else if (owners & 2) {
        actor->tracking.target = &((struct SceneActorChasePlayer *)gPlayers[1])->target;
        flagged = 1;
    } else if (owners & 4) {
        actor->tracking.target = &((struct SceneActorChasePlayer *)gPlayers[2])->target;
        flagged = 1;
    } else if (owners & 8) {
        actor->tracking.target = &((struct SceneActorChasePlayer *)gPlayers[3])->target;
        flagged = 1;
    } else if (owners & 0x10) {
        actor->tracking.target = 0;
        flagged = 1;
    } else if (owners & 0x20) {
        flagged = 1;
    }
    actor->field2E0 = owners;
    {
        u32 parameter = source->parameter34;
        u32 reaction = source->reactionFlags;
        actor->field2D8 = parameter;
        actor->field2DC = reaction;
    }
    if (flagged)
        AimAtContactTarget(actor);
}
static inline void ExtendContactTimer(struct SceneActorRegistryResetView *actor)
{
    actor->group.common.gate.counter += 0x12C;
    if (actor->group.common.gate.counter > actor->group.common.gate.duration)
        actor->group.common.gate.counter = actor->group.common.gate.duration;
}
static inline u8 IsPrimary(struct SceneActorRegistryResetView *actor)
{
    if (actor->object == (void *)1)
        return 1;
    return 0;
}
static inline u8 IsUnavailable(struct SceneActorRegistryResetView *actor, u16 **motion)
{
    u16 *flags;
    u32 mask;
    u32 disabled;
    mask = 0x40;
    flags = &actor->flags2D4;
    disabled = *flags & mask;
    *motion = flags;
    if (disabled)
        goto unavailable;
    if (SceneActorPlacement_ReadFlags32((void *)actor, 0x20000))
        goto unavailable;
    if (actor->health > 0)
        goto available;
unavailable:
    return 1;
available:
    return 0;
}
static inline void SpawnContactParts(struct SceneActorRegistryActor *actor)
{
    union SceneActorRegistryPackedPosition velocity, bounds, position;
    s32 i;
    for (i = 0; i < 3; i++) {
        union RandomTableView table = {gRandomTable};
        const struct ContactPosition *origin;
        velocity.packed.x = ((s8)SceneActorEffects_NextSample(table.samples) & 15) - 7;
        velocity.packed.y = (SceneActorEffects_NextSample(table.samples) & 31) - 16;
        velocity.packed.z = ((s8)SceneActorEffects_NextSample(table.samples) & 15) - 7;
        bounds.packed.x = 5;
        bounds.packed.y = 10;
        bounds.packed.z = 5;
        origin = actor->effectOrigin;
        position.packed.x = origin->x;
        position.packed.y = origin->y + 128;
        position.packed.z = origin->z;
        EffectGroup_SpawnPart((void *)actor->effectConfig, 3, &position.position,
                              &velocity.position, &bounds.position, 40, 40);
    }
    actor->fieldAA = 0;
    actor->field2B8 = 0;
}

static inline void SetContactResult(struct SceneActorRegistryResetView *actor, u16 value)
{
    actor->reactionSoundVariant = value;
}
void SceneActorReaction_HandleColliderContact(struct SceneActorBehaviorContactView *source,
                                              struct SceneActorBehaviorContactView *destination,
                                              struct SceneActorRegistryResetView *actor)
{
    struct ContactCollider *primary;
    u16 *motion;
    u32 unavailable;
    u16 sourceFlags;
    s32 power;

    if (IsPrimary(actor)) {
        DEBUG_PRINT();
    }
    unavailable = IsUnavailable(actor, &motion);
    if (unavailable)
        return;
    primary = &actor->colliderA;
    if (((struct SceneActorRegistryActor *)actor)->field178) {
        if (actor->field2E6) {
            destination->remaining = unavailable;
            destination->mode = 0;
        }
        return;
    }
    {
        u32 mask = 4;
        if (primary->flags & mask)
            return;
    }
    {
        u32 mask = 1;
        sourceFlags = source->flags;
        if (!(sourceFlags & mask)) {
            u32 attack;
            {
                u32 mask = 0x20000;
                if (!(mask & source->parameter34))
                    return;
            }
            if (TestMotion(motion, 0xA007))
                return;
            if (actor->group.countdown5C8)
                return;
            actor->group.countdown5C8 = 0x3C;
            attack =
                SceneActorPlacement_ReadFlags32((struct SceneActorPlacementView *)actor, 0x8000000);
            if (attack) {
                u32 mask = 0x100;
                if ((destination->flags & mask) && (sourceFlags & mask)) {
                    s32 d = destination->direction - source->direction;
                    if (d < 0)
                        d = -d;
                    if (d <= 0x20) {
                        actor->field2B8 = source->unknown3C;
                        actor->field2E6 = 1;
                        *(u16 *)actor->group.unknown5CE = Div(actor->field2B8, 3);
                        {
                            u32 bit = 0x2000;
                            *motion = bit | *motion;
                        }
                        return;
                    }
                }
                destination->remaining = 0;
                SetContactResult(actor, 0);
                SelectContactTarget(source, actor);
                if (TestMotion(motion, 0x80))
                    ExtendContactTimer(actor);
                actor->field2E6 = 1;
                return;
            } else {
                destination->remaining = attack;
                SetContactResult(actor, attack);
                SelectContactTarget(source, actor);
            }
            if (TestMotion(motion, 0x80))
                ExtendContactTimer(actor);
            actor->field2E6 = 1;
            return;
        }
    }
    if (Test(gEngineFlags, 0x4800))
        sub_08220204(source, destination);
    else
        Collision_CalculateDamageResponse(source, destination);
    destination->parameter44 = source->parameter44;
    actor->field2E6 = destination->mode;
    if (ReadReactionFlags(0x40000, source)) {
        u8 *defense = &actor->field9F;
        power = source->power;
        power -= *defense;
        if (power < 0)
            power = 0;
        destination->power = power;
    }
    if (TestMotion(motion, 6)) {
        destination->remaining <<= 1;
        power = destination->power;
        power = power >= 0 ? power >> 1 : -(-power >> 1);
        destination->power = power;
    }
    SceneActor_SetContactReactionDirection(actor, source, destination);
    SelectContactTarget(source, actor);
    if (actor->field2E6 || ReadReactionFlags(0x40, source)) {
        if (TestMotion(motion, 0x80))
            ExtendContactTimer(actor);
    }
    {
        u32 mask = 0x800;
        if (Test(gGameState->flags868, mask) || ReadReactionFlags(0x40, source)) {
            destination->remaining = 0;
            SetContactResult(actor, 0);
            actor->field2E6 = 1;
        }
    }
    if (TestMotion(motion, 0x2000))
        SpawnContactParts((void *)actor);
    actor->reactionSoundVariant = destination->remaining;
    PlayerStats_AddContactHundredths(source, destination, actor->fieldA0);
    MarkContact(actor, 0x2000);
}
