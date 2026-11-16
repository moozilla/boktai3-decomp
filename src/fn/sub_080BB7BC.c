#define SceneActorReaction_RestoreControl SceneActorPlacement_ResetActorState
/* Tentative reaction name, based on the observed fields and calls. */
#include "global.h"
#include "scene_actor_reaction.h"
#include "scene_actor_script.h"
u8 SceneActorReaction_TrySink(struct ChaseActor *);
void SceneActorPlacement_ResetActorState(struct ChaseActor *, const u16 *);
s32 Mod(s32, s32);
static inline u8 Test(u32 value, u32 mask)
{
    if (value & mask)
        return 1;
    return 0;
}
static inline u32 StateFlag(struct SceneActorScriptPropertiesView *state, u32 mask)
{
    return state->flags6DC & mask;
}
static inline void SetColliderFlag(struct ContactCollider *collider)
{
    collider->flags |= 4;
}
void SceneActorReaction_TickHiddenTimeout(struct ChaseActor *p, s32 elapsed)
{
    struct SceneActorScriptPropertiesView *state = p->base.context;
    u16 mode;
    if (SceneActorReaction_TakePrepared(p)) {
        u32 mask;
        SetColliderFlag(&p->base.colliderB);
        mask = 4;
        SetColliderFlag(&p->base.colliderA);
        ((struct ContactCollider *)((u8 *)state + 0x624))->flags |= mask;
        ((struct ContactCollider *)((u8 *)state + 0x678))->flags |= mask;
        SceneActorAnimation_SelectDirectionalEntry(&p->base, 0x11);
        ActorShadow_Disable((u8 *)p + 0x320);
    } else {
        if (Mod(elapsed, 10) == 0 && SceneActorReaction_TrySink(p))
            return;
        if (Test(StateFlag(state, 2), ~0))
            return;
        if (state->parameter6D2 > 19999)
            return;
        if (state->parameter6D2 >= elapsed)
            return;
        mode = 1;
        SceneActorPlacement_ResetActorState(p, &mode);
    }
}
