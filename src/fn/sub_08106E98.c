#include "tile_trigger.h"
#include "scene_actor_chase.h"
#include "monster_particle_random.h"

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

#ifdef NONMATCHING
// fuzzy: 74.2%
struct ContactEffectGroup {
    u8 unknown0[8];
    const struct ContactPosition *position;
};

static inline u8 ContactActor_IsControlled(struct SceneActorRegistryResetView *actor)
{
    if (actor->object == (void *)1)
        return 1;
    return 0;
}

void TileTrigger_ApplyContactEffects(struct TileTrigger *self)
{
    struct Player *players[4];
    u8 inRange[4];
    struct ContactPackedVector radius, velocity, scale, position;
    u32 cursor;
    s32 tetherInRange, actorFound, i;
    struct SceneActorRegistryResetView *actor;

    cursor = 0;
    radius.z = 128;
    radius.x = 128;
    radius.y = 16;
    for (i = 3; i >= 0; i--)
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
        u16 *timer = &actor->fieldAA;
        if ((u8)(actor->field2B5 - 0x16) <= 1)
            continue;
        Chase_SetCtl(actor, SceneActorReaction_SinkAndRemove, 0x16);
        Chase_SetState(actor, 3, 0, 0);
        actor->field2B8 = 0;
        if (ContactActor_IsControlled(actor) && Chase_Test(actor->flags2D4, 0x2000)) {
            u16 *cooldown = &actor->field2B8;
            struct ContactEffectGroup *group = (void *)((u8 *)actor + 0x47C);

            for (i = 0; i < 3; i++) {
                velocity.x = (ParticleRandom_ReadDirectSample(gRandomTable) & 15) - 7;
                velocity.y = (ParticleRandom_ReadDirectSample(gRandomTable) & 31) - 16;
                velocity.z = (ParticleRandom_ReadDirectSample(gRandomTable) & 15) - 7;
                scale.x = 5;
                scale.y = 10;
                scale.z = 5;
                position.x = group->position->x;
                position.y = group->position->y + 128;
                position.z = group->position->z;
                EffectGroup_SpawnPart((struct EffectGroup *)group, 3, (u32)&position,
                                      (u32)&velocity, (u32)&scale, 40, 40);
            }
            *timer = 0;
            *cooldown = 0;
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
#else
INCLUDE_ASM("asm/nonmatching", TileTrigger_ApplyContactEffects);
#endif
