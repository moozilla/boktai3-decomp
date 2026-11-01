#include "tile_trigger.h"
#include "scene_actor_chase.h"
#include "ram.h"

struct Player;
struct EffectGroup;
struct ScriptTriggerActor;
struct Player *SceneActorQuery_FindPlayerInBounds(const struct SceneVec *, const struct SceneVec *,
                                                  s32);
struct SceneActorRegistryActor *sub_0806F06C(const struct ContactPosition *,
                                             const struct ContactPosition *, u32 *);
u8 TileTrigger_IsTetherNearby(struct TileTrigger *);
s32 Player_BeginDirectionalCue(struct Player *, s32);
void TetherActor_BeginDetachedState(u32);
void ScriptTriggerActor_DispatchPendingScripts(struct ScriptTriggerActor *);
s32 EffectGroup_SpawnPart(struct EffectGroup *, u32, u32, u32, u32, u32, u32);
void SceneActorReaction_SinkAndRemove(struct SceneActorRegistryResetView *, s32);

static inline u32 ContactActor_ReadMotionFlags(const struct SceneActorRegistryResetView *actor,
                                               u32 mask)
{
    return mask & actor->flags2D4;
}
static inline u8 ContactActor_IsControlled(struct SceneActorRegistryResetView *actor)
{
    if (actor->object == (void *)1)
        return 1;
    return 0;
}

static inline u32 TileContact_NextParticleSample(u32 *cursor, const u16 *table)
{
    u32 index = (*cursor + 1) & 1023;
    *cursor = index;
    return table[index];
}
static inline void TileContact_EmitThreeParts(struct SceneActorRegistryActor *actor)
{
    s32 i;
    u16 sample;
    const void *source;
    struct ContactPackedVector velocity, scale, position;
    for (i = 0; i < 3; i++) {
        source = gRandomTable;
        sample = TileContact_NextParticleSample(&gRandomIndex, source);
        velocity.x = (sample & 15) - 7;
        velocity.y = (TileContact_NextParticleSample(&gRandomIndex, source) & 31) - 16;
        velocity.z = (TileContact_NextParticleSample(&gRandomIndex, source) & 15) - 7;
        scale.x = 5;
        scale.y = 10;
        scale.z = 5;
        position.x = actor->effectOrigin->x;
        position.y = actor->effectOrigin->y + 128;
        position.z = actor->effectOrigin->z;
        source = actor->effectConfig;
        EffectGroup_SpawnPart((struct EffectGroup *)source, 3, (u32)&position, (u32)&velocity,
                              (u32)&scale, 40, 40);
    }
    actor->fieldAA = 0;
    actor->field2B8 = 0;
}
void TileTrigger_ApplyContactEffects(struct TileTrigger *self)
{
    struct Player *players[4];
    u8 inRange[4];
    struct ContactPackedVector radius;
    u32 cursor;
    s32 tetherInRange, actorFound, i;
    struct SceneActorRegistryResetView *actor;
    void (*callback)(struct SceneActorRegistryResetView *, s32);

    callback = SceneActorReaction_SinkAndRemove;
    cursor = 0;
    radius.z = 128;
    radius.x = 128;
    radius.y = 16;
    for (i = 0; i < 4; i++)
        inRange[i] = 0;
    actorFound = 0;
    tetherInRange = 0;
    for (i = 0; i < 4; i++) {
        players[i] = SceneActorQuery_FindPlayerInBounds(&self->position,
                                                        (const struct SceneVec *)&radius, i);
        if (players[i]) {
            inRange[i] = 1;
            self->triggerFlags |= 0x80;
        }
    }
    if (TileTrigger_IsTetherNearby(self)) {
        tetherInRange = 1;
        self->triggerFlags |= 0x100;
    }
    while ((actor = (void *)sub_0806F06C((const struct ContactPosition *)&self->position,
                                         (const struct ContactPosition *)&radius, &cursor)) !=
           NULL) {
        if ((u8)(actor->field2B5 - 0x16) <= 1)
            continue;
        {
            u8 kind = 0x16;
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
        Chase_SetState(actor, 3, 0, 0);
        actor->field2B8 = 0;
        if (ContactActor_IsControlled(actor) && ContactActor_ReadMotionFlags(actor, 0x2000)) {
            TileContact_EmitThreeParts((struct SceneActorRegistryActor *)actor);
        }
        actorFound = 1;
    }
    if (inRange[0] || inRange[1] || inRange[2] || inRange[3]) {
        if (gEngineFlags & 0x800) {
            if (!(self->triggerFlags & 0x10)) {
                self->state = 5;
                self->forced = 1;
            }
        } else {
            self->state = 7;
        }
        if (tetherInRange) {
            for (i = 0; i < 4; i++)
                if (inRange[i] && Player_BeginDirectionalCue(players[i], 1))
                    self->triggerFlags |= 0x400;
            TetherActor_BeginDetachedState(1);
        } else {
            for (i = 0; i < 4; i++)
                if (inRange[i] && Player_BeginDirectionalCue(players[i], 0))
                    self->triggerFlags |= 0x400;
        }
    } else if (tetherInRange) {
        ScriptTriggerActor_DispatchPendingScripts((struct ScriptTriggerActor *)self);
        TetherActor_BeginDetachedState(0);
        self->state = 5;
        self->forced = 1;
    } else if (actorFound && !(self->triggerFlags & 0x10)) {
        self->state = 5;
        self->forced = 1;
    }
}
