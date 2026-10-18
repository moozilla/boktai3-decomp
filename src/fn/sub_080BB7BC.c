#define SceneActorReaction_TickHiddenTimeout sub_080BB7BC
#define SceneActorReaction_RestoreControl    sub_080772C0
/* Tentative reaction name, based on the observed fields and calls. */
#include "global.h"
#include "scene_actor_reaction.h"
#include "scene_actor_script.h"
u8 sub_080B8B84(struct SceneActorRegistryResetView *);
void sub_080772C0(struct SceneActorRegistryResetView *, const u16 *);
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
void sub_080BB7BC(struct SceneActorRegistryResetView *p, s32 elapsed)
{
    struct SceneActorScriptPropertiesView *state = p->context;
    u16 mode;
    if (SceneActorReaction_TakePrepared(p)) {
        u32 mask;
        SetColliderFlag(&p->colliderB);
        mask = 4;
        SetColliderFlag(&p->colliderA);
        ((struct ContactCollider *)((u8 *)state + 0x624))->flags |= mask;
        ((struct ContactCollider *)((u8 *)state + 0x678))->flags |= mask;
        SceneActorAnimation_SelectDirectionalEntry((struct SceneActorAnimationView *)p, 0x11);
        ActorShadow_Disable((u8 *)p + 0x320);
    } else {
        if (Mod(elapsed, 10) == 0 && sub_080B8B84(p))
            return;
        if (Test(StateFlag(state, 2), ~0))
            return;
        if (state->parameter6D2 > 19999)
            return;
        if (state->parameter6D2 >= elapsed)
            return;
        mode = 1;
        sub_080772C0(p, &mode);
    }
}
