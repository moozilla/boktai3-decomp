// MATCHING_CONCESSION: register-pinning
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
    if (kind == 1)
        goto slopeZ;
    if (kind == 2)
        goto slopeX;
    goto done;
slopeZ:
    height -= *((u8 *)position + 4);
    goto done;
slopeX:
    height -= *(u8 *)position;
done:;
    return height;
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
        u32 height;
        u32 mask = 0x4000;
        u32 flags;
        flags = actor->flags2D4;
        if (!(flags & mask)) {
            u32 playerRelative;
            if (category - 0x11 <= 3 || category == 0x17)
                playerRelative = 1;
            else
                playerRelative = 0;
            asm("" : "+r"(playerRelative));
            if (playerRelative) {
                height = (u16)gUnk_02000580[0]->position.y;
                goto haveHeight;
            }
        }
        {
            register u32 offset asm("r3") = 0x2f4;
            u32 index;
            asm("" : "+r"(offset));
            {
                register u16 *field asm("r0") = (void *)((u8 *)actor + offset);
                index = *field;
            }
            height = TerrainHeight(actor, index);
        }
    haveHeight:;
        *(u16 *)((u8 *)actor + 0x2be) = height;
    }
    {
        u32 mask = 0x20;
        u32 preserveY;
        if (*(u16 *)((u8 *)actor + 0x2c0) & mask) {
            preserveY = 1;
            asm("" : "+r"(preserveY));
        } else
            preserveY = 0;
        if (!preserveY) {
            u16 height = *(u16 *)((u8 *)actor + 0x2be);
            actor->position.y = height;
            {
                u32 offset = 0x2ea;
                {
                    u16 *field = (void *)((u8 *)actor + offset);
                    actor->position.y = height + *field;
                }
            }
        }
    }
    if (actor->drawableKind == 0)
        ((struct SceneActorRegistrySequenceDrawable *)actor->drawable)->position = actor->position;
    else
        ((struct SceneActorRegistryTileDrawable *)actor->drawable)->position = actor->position;
}
