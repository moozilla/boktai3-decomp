#include "scene_actor_registry.h"
#include "player.h"
u8 *TerrainModifiers_FindCellOverride(s32, s32);
static inline u32 TerrainHeight(struct SceneActorRegistryActor *actor, u32 index)
{
    const struct ContactPosition *position = &actor->position;
    u8 *cell = TerrainModifiers_FindCellOverride(index, 1);
    u32 kind, height;
    if (cell)
        cell += 4;
    else {
        struct EngineMap *map = gTerrainMap;
        u32 offset = index * 4 + 12;
        cell = map->cells + offset;
    }
    kind = *cell >> 4;
    height = (*cell & 15) << 8;
    switch (kind) {
        case 1:
            height -= *((u8 *)position + 4);
            break;
        case 2:
            height -= *(u8 *)position;
            break;
    }
    return height;
}
static inline u8 IsPlayerRelativeCategory(u32 c)
{
    if (c - 0x11 <= 3 || c == 0x17)
        return 1;
    return 0;
}
static inline u8 HasSceneFlag(struct SceneActorRegistryActor *actor, u32 mask)
{
    if (*(u16 *)((u8 *)actor + 0x2c0) & mask)
        return 1;
    return 0;
}
void SceneActorMovement_UpdateHeight(struct SceneActorRegistryActor *actor)
{
    u32 category = actor->category;
    if (category == 0x16) {
        u32 height = TerrainHeight(actor, *(u16 *)((u8 *)actor + 0x2f4));
        u16 *ground = (void *)((u8 *)actor + 0x2be);
        if (*ground != height) {
            u16 *displacement = (void *)((u8 *)actor + 0x2ea);
            *displacement -= height - *ground;
        }
        *ground = height;
    } else {
        u32 mask = 0x4000;
        if (!(actor->flags2D4 & mask) && IsPlayerRelativeCategory(category)) {
            u32 height = (u16)gPlayers[0]->position.y;
            *(u16 *)((u8 *)actor + 0x2be) = height;
        } else {
            u32 height = TerrainHeight(actor, *(u16 *)((u8 *)actor + 0x2f4));
            *(u16 *)((u8 *)actor + 0x2be) = height;
        }
    }
    if (!HasSceneFlag(actor, 0x20)) {
        u16 height = *(u16 *)((u8 *)actor + 0x2be);
        actor->position.y = height;
        actor->position.y = height + *(u16 *)((u8 *)actor + 0x2ea);
    }
    if (actor->drawableKind == 0)
        ((struct SceneActorRegistrySequenceDrawable *)actor->drawable)->position = actor->position;
    else
        ((struct SceneActorRegistryTileDrawable *)actor->drawable)->position = actor->position;
}
