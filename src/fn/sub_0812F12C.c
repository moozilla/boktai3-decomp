#include "global.h"
#include "scene_actor_registry.h"
#include "game_state.h"
#include "ram.h"
extern u32 gUnk_02000560;
void SceneActorPlacement_HideActor(struct SceneActorRegistryActor *, s32);
void SceneActorPlacement_ShowActor(struct SceneActorRegistryActor *, s32);
void sub_08075588(struct SceneActorRegistryActor *);
void sub_08074EB0(struct SceneActorRegistryActor *);
void SceneActorPlacement_ConsumeCallbackFlag(struct SceneActorRegistryActor *);
void SceneActorPlacement_ConsumeShakeFlags(struct SceneActorRegistryActor *);
s32 SceneActorMovement_RequestAvailableAction(struct SceneActorRegistryActor *);
void SceneActorTarget_OnBindingChanged(struct SceneActorRegistryActor *);
static inline u8 sameRegion(u16 *region)
{
    if (gGameState->region == *region)
        return 1;
    return 0;
}
static inline u32 flags16(struct SceneActorRegistryActor *p, u32 mask)
{
    return p->flags2D4 & mask;
}
static inline u32 flags32(struct SceneActorRegistryActor *p, u32 mask)
{
    return p->flags2D0 & mask;
}
static inline u8 unavailable(struct SceneActorRegistryActor *p)
{
    if (p) {
        if (flags16(p, 0x40) || flags32(p, 0x20000) || p->health <= 0)
            return 1;
    }
    return 0;
}
static inline u32 test(u32 mask, u32 flags)
{
    return mask & flags;
}
static inline void dispatch(void **callback, void *context)
{
    Callback_InvokeActor(context, *callback);
}
static inline u32 engineFlags(u32 mask)
{
    return (gEngineFlags | gScenePauseFlags) & mask;
}
s32 SceneActor_UpdateActiveCallbacks(struct SceneActorRegistryActor *p)
{
    if (engineFlags(0x400))
        return 0;
    if (test(4, gUnk_02000560))
        return 0;
    if (!(test(2, p->flags2CC))) {
        if (!sameRegion(&p->regionId)) {
            SceneActorPlacement_HideActor(p, 0);
            return 0;
        }
        if (!unavailable(p))
            Callback_InvokeActor(p, p->activeCallback264);
        SceneActorPlacement_ConsumeCallbackFlag(p);
        sub_08075588(p);
        dispatch(&p->contextCallback274, p->context);
        SceneActorMovement_RequestAvailableAction(p);
        sub_08074EB0(p);
        SceneActorPlacement_ConsumeShakeFlags(p);
        SceneActorTarget_OnBindingChanged(p);
        return 1;
    } else {
        if (!(p->flags2CC & 1) && sameRegion(&p->regionId))
            SceneActorPlacement_ShowActor(p, 0);
        SceneActorTarget_OnBindingChanged(p);
    }
    return 0;
}
