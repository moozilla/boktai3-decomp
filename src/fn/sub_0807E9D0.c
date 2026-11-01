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
static inline s32 SelectContactTarget(struct SceneActorBehaviorContactView *source,
                                      struct SceneActorRegistryResetView *actor, u32 parameter)
{
    s32 flagged = 0;
    u16 owners = source->kind;
    u32 reaction = source->reactionFlags;
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
    actor->field2D8 = parameter;
    actor->field2DC = reaction;
    return flagged;
}

static inline void AimAtContactTarget(struct SceneActorRegistryResetView *actor)
{
    struct SceneActorChaseTarget *target;
    if (Test(actor->flags2C0, 2))
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

static inline void ExtendContactTimer(struct SceneActorRegistryResetView *actor)
{
    actor->group.common.gate.counter += 0x12C;
    if (actor->group.common.gate.counter > actor->group.common.gate.duration)
        actor->group.common.gate.counter = actor->group.common.gate.duration;
}

#ifdef NONMATCHING
// fuzzy: 38.1%
void sub_0807E9D0(struct SceneActorBehaviorContactView *source,
                  struct SceneActorBehaviorContactView *destination,
                  struct SceneActorRegistryResetView *actor)
{
    u32 attackFlag;
    s32 flagged;

    if (SceneActorPlacement_IsUnavailable((void *)actor))
        return;

    if (((struct SceneActorRegistryActor *)actor)->field178 != 0) {
        if (actor->field2E6 == 0)
            return;
        destination->remaining = 0;
        destination->mode = 0;
        return;
    }
    if (actor->colliderA.flags & 4)
        return;
    if (source->flags & 1)
        goto damage;

    if (!(source->parameter34 & 0x20000))
        return;
    if (actor->flags2D4 & 0xA007)
        return;
    if (actor->group.countdown5C8 != 0)
        return;
    actor->group.countdown5C8 = 0x3C;
    attackFlag = actor->flags2D0 & 0x8000000;
    if (attackFlag) {
        if ((destination->flags & 0x100) && (source->flags & 0x100)) {
            s32 d = destination->direction - source->direction;
            if (d < 0)
                d = -d;
            if (d <= 0x20) {
                actor->field2B8 = source->unknown3C;
                actor->field2E6 = 1;
                *(u16 *)actor->group.unknown5CE = Div(actor->field2B8, 3);
                actor->flags2D4 |= 0x2000;
                return;
            }
        }
        destination->remaining = 0;
        actor->reactionSoundVariant = 0;
        flagged = SelectContactTarget(source, actor, source->parameter34);
        if (flagged)
            AimAtContactTarget(actor);
    } else {
        destination->remaining = attackFlag;
        actor->reactionSoundVariant = attackFlag;
        flagged = SelectContactTarget(source, actor, source->parameter34);
        if (flagged)
            AimAtContactTarget(actor);
    }
    if (actor->flags2D4 & 0x80)
        ExtendContactTimer(actor);
    actor->field2E6 = 1;
    return;

damage:
    if (gEngineFlags & 0x4800)
        sub_08220204(source, destination);
    else
        Collision_CalculateDamageResponse(source, destination);
    destination->parameter44 = source->parameter44;
    {
        u16 t = destination->mode;
        actor->field2E6 = t;
    }
    if (source->reactionFlags & 0x40000) {
        s32 v = source->power - actor->field9F;
        destination->power = v < 0 ? 0 : v;
    }
    if (actor->flags2D4 & 6) {
        destination->remaining = destination->remaining << 1;
        {
            s32 power = destination->power;
            destination->power = power >= 0 ? power >> 1 : -(-power >> 1);
        }
    }
    SceneActor_SetContactReactionDirection(actor, source, destination);
    flagged = SelectContactTarget(source, actor, source->parameter34);
    if (flagged)
        AimAtContactTarget(actor);
    if (actor->field2E6 != 0 || (source->reactionFlags & 0x40)) {
        if (actor->flags2D4 & 0x80)
            ExtendContactTimer(actor);
    }
    if (Test(gGameState->flags868, 0x800) || (source->reactionFlags & 0x40)) {
        destination->remaining = 0;
        actor->reactionSoundVariant = 0;
        actor->field2E6 = 1;
    }
    if (actor->flags2D4 & 0x2000) {
        SceneActorEffects_SpawnContactParts((void *)actor);
    }
    actor->reactionSoundVariant = destination->remaining;
    PlayerStats_AddContactHundredths(source, destination, actor->fieldA0);
    actor->flags2CC |= 0x2000;
    return;
}
#else
INCLUDE_ASM("asm/nonmatching", sub_0807E9D0);
#endif
