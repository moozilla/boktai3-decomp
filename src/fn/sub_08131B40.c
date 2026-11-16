#include "global.h"
#include "scene_actor_reset.h"
#include "ram.h"
extern u32 gUnk_02000560;
static inline u32 IsSuspended(void)
{
    u32 mask = 0x400;
    return (gEngineFlags | gScenePauseFlags) & mask;
}
static inline u32 HasOverride(void)
{
    u32 mask = 4;
    return gUnk_02000560 & mask;
}
#define SceneActorBehavior_DispatchTable284OrEnterState2 sub_08131B40
void SceneActorBehavior_DispatchTable284OrEnterState2(struct SceneActorRegistryResetView *actor)
{
    if (IsSuspended() || HasOverride()) {
        u32 state = 2;
        u32 one = 1;
        u32 zero;
        u8 *p = &actor->base.state2A8;
        zero = 0;
        *p = state;
        actor->base.phase2A9 = zero;
        actor->base.behavior2AA = one;
        actor->base.counter2AC = zero;
        actor->base.field2B1 = one;
    } else {
        Callback_InvokeActor(actor, actor->base.callbacks284[actor->base.behavior2AA]);
    }
}
