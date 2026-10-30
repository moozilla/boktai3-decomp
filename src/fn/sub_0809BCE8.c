#include "global.h"
#include "scene_actor_reaction.h"
#include "script_utility.h"
void *BestiaryDisplay_GetEntryForActor(u32, u32);
void GameState_SetEventFlag(void *);
void SceneActorControl_ConsumeLiftFlagPenalty(void *);
static inline void SetControl(struct SceneActorRegistryResetView *actor, u32 kind, void *target)
{
    void *callback = target;
    u32 type = kind;
    actor->field2B2 = 1;
    actor->field2B0 = 0;
    actor->field2B5 = type;
    actor->target = callback;
    actor->field2C8 = 0;
    {
        u32 *flags = &actor->flags114;
        s32 mask = -2;
        *flags &= mask;
    }
    actor->word104 = 0;
}
static inline void SetState(struct SceneActorRegistryResetView *actor, u32 state, u32 phase,
                            u32 behavior)
{
    actor->state2A8 = state;
    actor->phase2A9 = phase;
    actor->behavior2AA = behavior;
    actor->counter2AC = 0;
    actor->field2B1 = 1;
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
void Lun_UpdateDefeat(struct SceneActorRegistryResetView *actor)
{
    s32 values[8];
    struct ScriptUtilityArguments arguments;
    u16 script;
    if (TakeFlag(&actor->field2B2)) {
        SceneActorAnimation_SelectProfileEntry((void *)actor, 10);
        SceneActorPlacement_PlayCategoryEventSound(actor);
        {
            u32 *p = &actor->flags2D0;
            u32 m = ~0x100000;
            *p &= m;
        }
        {
            struct ContactCollider *c = &actor->colliderB;
            c->flags |= 4;
        }
        {
            struct ContactCollider *c = &actor->colliderA;
            c->flags |= 4;
        }
        {
            u32 m = 0x800;
            actor->flags2C0 = m | actor->flags2C0;
        }
        SceneActorControl_ConsumeLiftFlagPenalty(actor);
    }
    if (actor->field2B3) {
        actor->field2B0 = 1;
        SceneActorBehavior_CallTerrainScript(actor);
        script = *(u16 *)((u8 *)actor + 0x1F8);
        if (script) {
            values[0] = actor->body.position.x;
            values[1] = actor->body.position.y;
            values[2] = actor->body.position.z;
            values[3] = *(s32 *)((u8 *)actor + 0x1FC);
            values[4] = *(s32 *)((u8 *)actor + 0x200);
            values[5] = *(s32 *)((u8 *)actor + 0x204);
            values[6] = actor->field2E0;
            values[7] = actor->body.id;
            arguments.count = 8;
            arguments.values = values;
            Script_Call(script, &arguments);
        }
        {
            u32 m = 0x4000;
            if (!(actor->flags2D4 & m) && (u32)(actor->category - 11) > 2)
                GameState_SetEventFlag(
                    BestiaryDisplay_GetEntryForActor(actor->category, actor->paletteOffset));
        }
        SetControl(actor, 16, SceneActorControl_OnIdle);
        SetState(actor, 1, 0, 2);
    }
}
