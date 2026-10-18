#include "global.h"
#include "scene_actor_placement.h"
void ActorShadow_Disable(void *);
static inline void ClearWord(u32 *p, u32 mask) { *p &= mask; }
static inline void ClearTile(struct SceneActorPlacementTileFlags *p, u32 mask) { p->flags &= mask; }
static inline void MarkCollider(struct ContactCollider *p) { p->flags = 4 | p->flags; }
void SceneActorPlacement_HideActor(struct SceneActorPlacementView *s)
{
    if (!SceneActorPlacement_IsUnavailable(s)) {
        u32 bit = 2;
        u32 flags = s->flags2CC;
        if (!(flags & bit)) {
            void *drawable;
            s->flags2CC = flags | bit;
            if (s->drawableKind == 0) {
                u32 *p = s->drawable;
                ClearWord(p, ~2);
                drawable = p;
            } else {
                struct SceneActorPlacementTileFlags *p;
                drawable = s->drawable;
                p = (void *)((u8 *)drawable + 0x20);
                ClearTile(p, ~2);
            }
            {
                u32 one = 1;
                if (s->drawableKind == 0) {
                    *(u32 *)drawable |= one;
                } else {
                    struct SceneActorPlacementTileFlags *p = (void *)((u8 *)drawable + 0x20);
                    p->flags |= one;
                }
            }
            MarkCollider(&((struct SceneActorRegistryResetView *)s)->colliderA);
            if (s->shadow320[0]) ActorShadow_Disable(s->shadow320);
        }
    }
}
