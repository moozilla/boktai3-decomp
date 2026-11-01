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
static inline u32 Unavailable(struct SceneActorRegistryResetView *actor)
{
    return SceneActorPlacement_IsUnavailable((struct SceneActorPlacementView *)actor);
}
static inline u32 Motion(struct SceneActorRegistryResetView *actor, u32 mask)
{
    return SceneActorPlacement_ReadFlags16((struct SceneActorPlacementView *)actor, mask);
}
static inline u16 *MotionFlags(struct SceneActorRegistryResetView *actor)
{
    return &actor->flags2D4;
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
#ifdef NONMATCHING
// fuzzy: 93.1%
// 51 insns differ. The ROM starts with a dead `ldr r0, [r6]` (actor->object) and no null check:
// delete_null_pointer_checks removed IsUnavailable's `if (actor)` after a dereference, yet that load
// survived flow. The ROM also keeps IsUnavailable's flags2D4 address for every later flag test (sl),
// which LCM only does when that occurrence is upward-exposed in its block; here `motion` is assigned
// first instead, so the 0x40 mask loads after the address and source and motion swap sb/sl.
void SceneActorReaction_HandleColliderContact(struct SceneActorBehaviorContactView *source,
                                              struct SceneActorBehaviorContactView *destination,
                                              struct SceneActorRegistryResetView *actor)
{
    struct ContactCollider *primary;
    u16 *motion;
    u32 unavailable;
    u32 sourceFlags;
    u32 mask;
    s32 power;
    motion = MotionFlags(actor);
    unavailable = Unavailable(actor);
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
    sourceFlags = source->flags;
    if (!(sourceFlags & 1)) {
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
            actor->reactionSoundVariant = 0;
            SelectContactTarget(source, actor);
        } else {
            destination->remaining = attack;
            actor->reactionSoundVariant = attack;
            SelectContactTarget(source, actor);
        }
        if (TestMotion(motion, 0x80))
            ExtendContactTimer(actor);
        actor->field2E6 = 1;
        return;
    }
    if (Test(gEngineFlags, 0x4800))
        sub_08220204(source, destination);
    else
        Collision_CalculateDamageResponse(source, destination);
    destination->parameter44 = source->parameter44;
    actor->field2E6 = destination->mode;
    if (source->reactionFlags & 0x40000) {
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
    if (actor->field2E6 || (source->reactionFlags & 0x40)) {
        if (TestMotion(motion, 0x80))
            ExtendContactTimer(actor);
    }
    mask = 0x800;
    if (Test(gGameState->flags868, mask) || (source->reactionFlags & 0x40)) {
        destination->remaining = 0;
        actor->reactionSoundVariant = 0;
        actor->field2E6 = 1;
    }
    if (TestMotion(motion, 0x2000))
        SceneActorEffects_SpawnContactParts((void *)actor);
    actor->reactionSoundVariant = destination->remaining;
    PlayerStats_AddContactHundredths(source, destination, actor->fieldA0);
    MarkContact(actor, 0x2000);
}
#else
INCLUDE_ASM("asm/nonmatching", SceneActorReaction_HandleColliderContact);
#endif
