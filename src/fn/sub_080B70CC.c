#define SceneActorReaction_SelectTileParameter     sub_080B70CC
#define gSceneActorReactionCategory7TileParameters gUnk_08606078
#define gSceneActorReactionDefaultTileParameters   gUnk_08606058
#include "global.h"
#include "scene_actor_reset.h"
extern const u16 gUnk_08606078[2][8];
extern const u16 gUnk_08606058[2][8];
static inline void SetTileFlag(struct SceneActorRegistryResetView *p, u32 mask)
{
    p->flags2D0 |= mask;
}
u16 sub_080B70CC(struct SceneActorRegistryResetView *actor)
{
    struct SceneActorRegistryTileDrawable *drawable = actor->drawable;
    u16 *tile = (u16 *)((u8 *)drawable + 0x20);
    u16 *second = tile;
    u32 variant;
    if (tile[6] == 0) {
        u16 mask = 0x80;
        variant = (u16)(actor->flags2D4 & mask) != 0;
        SetTileFlag(actor, 0x10000);
        if (actor->category == 7) {
            return gUnk_08606078[variant][tile[10] & 7];
        } else {
            return gUnk_08606058[variant][second[10] & 7];
        }
    }
    return 0;
}
