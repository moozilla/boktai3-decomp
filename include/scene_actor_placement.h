#ifndef GUARD_SCENE_ACTOR_PLACEMENT_H
#define GUARD_SCENE_ACTOR_PLACEMENT_H
#include "scene_actor_reset.h"
#include "scene_actor_terrain.h"
/* Reset/placement phase; resetControl220 is observed separately from savedTarget22C. */
struct SceneActorPlacementView {
    void *object, *drawable;
    u8 drawableKind, unknown9[0x50];
    u8 direction, unknown5A[2];
    struct ContactPosition position;
    u8 unknown64[0x34];
    u8 category, unknown99[3];
    s16 health;
    u8 unknown9E[0x66];
    u32 field104;
    u8 unknown108[0xC];
    u32 flags114;
    u8 unknown118[0x60];
    u16 field178;
    u8 unknown17A[0x6C];
    u16 paletteVariant;
    u8 unknown1E8[0x34];
    const void *control, *resetControl220;
    u8 unknown224[0x18];
    void *callback23C;
    u8 unknown240[0x68];
    u8 state2A8, phase2A9, behavior2AA, unknown2AB;
    u32 counter2AC;
    u8 field2B0, field2B1, field2B2, restart;
    u8 enabled, field2B5, field2B6, selection;
    u16 field2B8;
    u8 unknown2BA[6];
    u16 flags2C0, field2C2;
    u32 profileMode;
    u16 field2C8;
    u8 unknown2CA[2];
    u32 flags2CC, flags2D0;
    u16 flags2D4, flags2D6;
    u8 unknown2D8[0xC];
    u16 field2E4, flags2E6;
    u8 unknown2E8[0xC];
    u16 terrainCell;
    u8 unknown2F6[0xDE];
    struct SceneActorRegistryControlGroup group;
    struct SceneActorRegistryRoute route;
};
void sub_080714E4(struct SceneActorRegistryActor *);
void sub_0807171C(struct SceneActorRegistryActor *);
void sub_080718D8(struct SceneActorRegistryActor *);
void sub_08077848(struct SceneActorPlacementView *, u32);
static inline u32 SceneActorPlacement_ReadFlags16(struct SceneActorPlacementView *actor, u32 mask)
{
    return actor->flags2D4 & mask;
}
static inline u32 SceneActorPlacement_ReadFlags32(struct SceneActorPlacementView *actor, u32 mask)
{
    return actor->flags2D0 & mask;
}
static inline u8 SceneActorPlacement_IsUnavailable(struct SceneActorPlacementView *actor)
{
    if (actor) {
        if (SceneActorPlacement_ReadFlags16(actor, 0x40) ||
            SceneActorPlacement_ReadFlags32(actor, 0x20000) || actor->health <= 0)
            return 1;
    }
    return 0;
}
static inline u8 SceneActorPlacement_TestFlags(const u16 *flags, u32 mask)
{
    if (*flags & mask)
        return 1;
    return 0;
}
static inline void SceneActorPlacement_ClearFlags(u16 *flags, u32 mask)
{
    *flags &= mask;
}
static inline u8 SceneActorPlacement_TestValue(u32 flags, u32 mask)
{
    if (mask & flags)
        return 1;
    return 0;
}
static inline u8 SceneActorPlacement_TestMask(u32 mask, u32 flags)
{
    if (mask & flags)
        return 1;
    return 0;
}
#endif
