#include "global.h"
#include "engine.h"
#include "ram.h"
#include "scene_vec.h"
#include "scene_actor_state.h"
#include "scene_actor_terrain_reaction.h"
#include "effect_group.h"
#include "scene_actor_registry.h"

struct TileTrigger {
    u32 flags;
    u8 unknown4[0x70];
    u8 animation[0x10];
    struct SceneVec position;
    u32 key;
    u8 unknown90[0x28];
    u16 sequence, delay, unknownBC;
    s16 state;
    u16 triggerFlags;
    u8 forced, frame;
};
struct Player;
struct Player *SceneActorQuery_FindPlayerInBounds(const struct SceneVec *, const struct SceneVec *,
                                                  s32);
struct SceneActorRegistryResetView *sub_0806F06C(const struct SceneVec *, const struct SceneVec *,
                                                 u32 *);
u8 TileTrigger_IsTetherNearby(struct TileTrigger *);
s32 Player_BeginDirectionalCue(struct Player *p, s32 mode);
void TetherActor_BeginDetachedState(s32 mode);
void ScriptTriggerActor_DispatchPendingScripts(struct TileTrigger *p);
void SceneActorReaction_SinkAndRemove(void);

static inline u8 IsControlled(struct SceneActorRegistryResetView *actor)
{
    if (actor->object == (void *)1)
        return 1;
    return 0;
}

static inline u32 RandomSample(const u16 *t, u32 i)
{
    return t[i];
}
static inline u16 NextRandomIndex(u32 i)
{
    return (i + 1) & 1023;
}
static inline void SpawnSinkParts(struct SceneActorRegistryActor *actor)
{
    union SceneActorRegistryPackedPosition velocity, bounds, position;
    s32 i;
    for (i = 0; i < 3; i++) {
        union RandomTableView table = {gRandomTable};
        u32 index = NextRandomIndex(gRandomIndex);
        velocity.packed.x = (RandomSample(table.samples, index) & 15) - 7;
        index = NextRandomIndex(index);
        velocity.packed.y = (RandomSample(table.samples, index) & 31) - 16;
        index = NextRandomIndex(index);
        gRandomIndex = index;
        velocity.packed.z = (RandomSample(table.samples, index) & 15) - 7;
        bounds.packed.x = 5;
        bounds.packed.y = 10;
        bounds.packed.z = 5;
        position.packed.x = actor->effectOrigin->x;
        position.packed.y = actor->effectOrigin->y + 128;
        position.packed.z = actor->effectOrigin->z;
        EffectGroup_SpawnPart((void *)actor->effectConfig, 3, &position.position,
                              &velocity.position, &bounds.position, 40, 40);
    }
    actor->fieldAA = 0;
    actor->field2B8 = 0;
}

static inline u32 HasFlags2D4(struct SceneActorRegistryResetView *actor, u32 mask)
{
    return mask & actor->flags2D4;
}

static inline u16 HoldsFlag10(struct TileTrigger *t)
{
    return t->triggerFlags & 0x10;
}
/* Active tick of a sinking tile trigger (tentative): flags players (0x80) and a
   tether (0x100) within 128x16 of the trigger, hands every actor in range to
   SceneActorReaction_SinkAndRemove (kind 0x16, state 3) with a spark burst for
   controlled actors, then sets the trigger state from what it found. */
void sub_08106E98(struct TileTrigger *trigger)
{
    u32 cursor;
    struct SceneVec radius;
    struct Player *players[4];
    u8 inRange[4];
    s32 anyTether;
    s32 found;
    s32 i;
    struct SceneActorRegistryResetView *actor;
    void *sink = SceneActorReaction_SinkAndRemove;

    cursor = 0;
    radius.z = 128;
    radius.x = 128;
    radius.y = 16;
    for (i = 0; i < 4; i++)
        inRange[i] = 0;
    found = 0;
    anyTether = 0;
    for (i = 0; i < 4; i++) {
        players[i] = SceneActorQuery_FindPlayerInBounds(&trigger->position, &radius, i);
        if (players[i]) {
            inRange[i] = 1;
            trigger->triggerFlags |= 0x80;
        }
    }
    if (TileTrigger_IsTetherNearby(trigger)) {
        anyTether = 1;
        trigger->triggerFlags |= 0x100;
    }
    while ((actor = sub_0806F06C(&trigger->position, &radius, &cursor)) != 0) {
        if ((u8)(actor->field2B5 - 0x16) <= 1)
            continue;
        SceneActorTerrain_InstallControl(actor, sink, 0x16);
        SceneActorTerrain_EnterState(actor, 3, 0, 0);
        actor->field2B8 = 0;
        if (IsControlled(actor) && HasFlags2D4(actor, 0x2000))
            SpawnSinkParts((struct SceneActorRegistryActor *)actor);
        found = 1;
    }
    if (inRange[0] || inRange[1] || inRange[2] || inRange[3]) {
        if (gEngineFlags & 0x800) {
            if (!HoldsFlag10(trigger)) {
                trigger->state = 5;
                trigger->forced = 1;
            }
        } else {
            trigger->state = 7;
        }
        if (anyTether) {
            for (i = 0; i < 4; i++) {
                if (inRange[i] && Player_BeginDirectionalCue(players[i], 1))
                    trigger->triggerFlags |= 0x400;
            }
            TetherActor_BeginDetachedState(1);
        } else {
            for (i = 0; i < 4; i++) {
                if (inRange[i] && Player_BeginDirectionalCue(players[i], 0))
                    trigger->triggerFlags |= 0x400;
            }
        }
    } else if (anyTether) {
        ScriptTriggerActor_DispatchPendingScripts(trigger);
        TetherActor_BeginDetachedState(0);
        trigger->state = 5;
        trigger->forced = 1;
    } else if (found) {
        if (!HoldsFlag10(trigger)) {
            trigger->state = 5;
            trigger->forced = 1;
        }
    }
}
