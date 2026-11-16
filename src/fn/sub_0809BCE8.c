#include "scene_actor_behavior.h"
#include "global.h"
#include "scene_actor_reaction.h"
#include "script_utility.h"
void *Bestiary_GetEntryForActor(u32, u32);
void Bestiary_UnlockEntry(void *);
void SceneActorControl_ConsumeLiftFlagPenalty(void *);
static inline void SetControl(struct ChaseActor *actor, u32 kind, void *target)
{
    void *callback = target;
    u32 type = kind;
    actor->base.field2B2 = 1;
    actor->base.field2B0 = 0;
    actor->base.field2B5 = type;
    actor->base.target = callback;
    actor->base.field2C8 = 0;
    {
        u32 *flags = &actor->base.flags114;
        s32 mask = -2;
        *flags &= mask;
    }
    actor->base.word104 = 0;
}
static inline void SetState(struct ChaseActor *actor, u32 state, u32 phase, u32 behavior)
{
    actor->base.state2A8 = state;
    actor->base.phase2A9 = phase;
    actor->base.behavior2AA = behavior;
    actor->base.counter2AC = 0;
    actor->base.field2B1 = 1;
}

static inline u8 TakeFlag(u8 *p)
{
    if (*p) {
        *p = 0;
        return 1;
    }
    return 0;
}
// confidence=low: Lun defeat callback selects death animation and runs its completion script.
void Lun_UpdateDefeat(struct ChaseActor *actor)
{
    s32 values[8];
    struct ScriptUtilityArguments arguments;
    u16 script;
    if (TakeFlag(&actor->base.field2B2)) {
        SceneActorAnimation_SelectProfileEntry((void *)actor, 10);
        SceneActorPlacement_PlayCategoryEventSound(actor);
        {
            u32 *p = &actor->base.flags2D0;
            u32 m = ~0x100000;
            *p &= m;
        }
        {
            struct ContactCollider *c = &actor->base.colliderB;
            c->flags |= 4;
        }
        {
            struct ContactCollider *c = &actor->base.colliderA;
            c->flags |= 4;
        }
        {
            u32 m = 0x800;
            actor->base.flags2C0 = m | actor->base.flags2C0;
        }
        SceneActorControl_ConsumeLiftFlagPenalty(actor);
    }
    if (actor->base.restart) {
        actor->base.field2B0 = 1;
        SceneActorBehavior_CallTerrainScript(actor);
        script = (u16)actor->base.script1F8;
        if (script) {
            values[0] = actor->base.body.position.x;
            values[1] = actor->base.body.position.y;
            values[2] = actor->base.body.position.z;
            values[3] = actor->base.parameters1FC[0];
            values[4] = actor->base.parameters1FC[1];
            values[5] = actor->base.parameters1FC[2];
            values[6] = actor->base.field2E0;
            values[7] = actor->base.body.id;
            arguments.count = 8;
            arguments.values = values;
            Script_Call(script, &arguments);
        }
        {
            u32 m = 0x4000;
            if (!(actor->base.flags2D4 & m) && (u32)(actor->base.category - 11) > 2)
                Bestiary_UnlockEntry(
                    Bestiary_GetEntryForActor(actor->base.category, actor->base.paletteOffset));
        }
        SetControl(actor, 16, SceneActorControl_OnIdle);
        SetState(actor, 1, 0, 2);
    }
}
