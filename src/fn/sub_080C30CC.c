#include "global.h"
#include "scene_actor_registry.h"
#include "game_state.h"
extern u32 gUnk_030053F0, gUnk_02000560;
void SceneActorPlacement_HideActor(struct SceneActorRegistryActor *, s32);
void SceneActorPlacement_ShowActor(struct SceneActorRegistryActor *, s32);
void SceneActorPlacement_ConsumeShakeFlags(struct SceneActorRegistryActor *);
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
    return (gEngineFlags | gUnk_030053F0) & mask;
}
void SceneActorBehavior_ActivatePendingControl(struct SceneActorRegistryActor *);
s32 SceneActorBehavior_DispatchPendingFlags(struct SceneActorRegistryActor *);
s32 SceneActorBehavior_UpdateTimersAndContacts(struct SceneActorRegistryActor *);
void SceneActorMotion_Update(struct SceneActorRegistryActor *);
void sub_08078EE8(struct SceneActorRegistryActor *);
static inline u8 pending(u8 *p)
{
    u32 mask = 0x10;
    if (*(u32 *)(p + 0x6DC) & mask)
        return 1;
    return 0;
}
s32 Skeleton_UpdateBehaviorCallbacks(struct SceneActorRegistryActor *p)
{
    u8 *state = (void *)p;
    struct SceneActorRegistryCallbackBlock *block = (void *)p->unknown3D4;
    if (engineFlags(0x400))
        return 0;
    if (test(4, gUnk_02000560))
        return 0;
    if (!(test(2, p->flags2CC))) {
        if (!sameRegion(&p->regionId)) {
            SceneActorPlacement_HideActor(p, 0);
            return 0;
        }
        if (!unavailable(p) && !pending(state)) {
            if (!(u8)Callback_InvokeActor(p, p->activeCallback264))
                Callback_InvokeActor(p, block->fallback);
        }
        SceneActorBehavior_DispatchPendingFlags(p);
        sub_08078EE8(p);
        dispatch(&p->contextCallback274, p->context);
        SceneActorBehavior_UpdateTimersAndContacts(p);
        SceneActorMotion_Update(p);
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
