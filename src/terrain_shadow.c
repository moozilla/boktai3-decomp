#include "terrain_shadow.h"

s32 ShadowList_Insert(struct TerrainShadowManager *manager, struct TerrainShadow *shadow)
{
    u32 active = shadow->active;
    struct TerrainShadow *first;
    s32 result;
    if (active != 0) {
        result = -1;
    } else {
        shadow->previous = (void *)active;
        first = manager->head;
        shadow->next = first;
        if (first) {
            first->previous = shadow;
        }
        manager->head = shadow;
        shadow->active = 1;
        result = 0;
    }
    return result;
}

s32 ShadowList_Remove(struct TerrainShadowManager *manager, struct TerrainShadow *shadow)
{
    struct TerrainShadow *previous = shadow->previous;
    struct TerrainShadow *next = shadow->next;
    s32 result;
    if (shadow->active == 0) {
        return -1;
    }
    if (previous) {
        previous->next = next;
    } else {
        manager->head = next;
    }
    if (next) {
        next->previous = previous;
    }
    result = 0;
    shadow->active = result;
    return result;
}

void ActorShadow_Enable(void *actor)
{
    struct TerrainShadow *shadow = actor;
    shadow->sprite.flags &= ~1;
    shadow->update = Shadow_UpdateTerrain;
    shadow->update(shadow);
}

void ActorShadow_Disable(void *actor)
{
    struct TerrainShadow *shadow = actor;
    shadow->sprite.flags |= 1;
    shadow->update = ActorShadow_UpdateNone;
}

void ActorShadow_UpdateNone(struct TerrainShadow *shadow)
{
}

void Shadow_UpdateTerrain(struct TerrainShadow *shadow)
{
    u32 *flags = &shadow->sprite.flags;
    struct SceneVec *ground = &shadow->sprite.position;
    struct SceneVec *source = shadow->source;
    int index = ShadowTerrain_CellIndex(source);
    if (shadow->flags & 1)
        ground->y = source->y;
    else {
        struct ShadowTerrainCell *terrain = ShadowTerrain_Cell(index);
        int slope = (terrain->height >> 2) >> 2;
        int height = (terrain->height & 15) << 8;
        switch (slope) {
            case 1:
                height -= (u8)source->z;
                break;
            case 2:
                height -= (u8)source->x;
                break;
        }
        ground->y = height;
    }
    if (source->y < ground->y || ground->y == 0 ||
        ShadowTerrain_HidesShadow(ShadowTerrain_Cell(index)))
        *flags |= 1;
    else {
        *flags &= ~1;
        ground->x = source->x;
        ground->z = source->z;
    }
}

u32 ShadowTask_Update(struct TerrainShadowManager *manager)
{
    struct TerrainShadow *shadow = manager->head;
    if (shadow) {
        do {
            shadow->update(shadow);
            shadow = shadow->next;
        } while (shadow);
    }
    return 0;
}

void ShadowTask_Destroy(void)
{
    gShadowManager = 0;
}

s32 ShadowTask_Init(struct TerrainShadowManager *manager, void *unused)
{
    gShadowManager = (u32)manager;
    manager->head = 0;
    manager->resource = AnimData_FindEntry(0x1C1A);
    return 0;
}

struct TerrainShadowManager *ShadowTask_Create(void *argument, s32 unused)
{
    struct TerrainShadowManager *manager = Task_Create(9, sizeof(*manager));
    if (manager != 0) {
        Task_SetCallbacks(manager, (void *)ShadowTask_Update, (void *)ShadowTask_Destroy);
        if (ShadowTask_Init(manager, argument) < 0) {
            Task_Kill(manager);
            return 0;
        }
    }
    return manager;
}

s32 Shadow_Initialize(struct TerrainShadow *shadow, struct SceneVec *source, s32 mode)
{
    struct ShadowTileSprite *sprite;
    s32 zero;
    if (!((struct TerrainShadowManager *)gShadowManager) && !ShadowTask_Create(0, 0))
        return -1;
    zero = 0;
    shadow->active = zero;
    shadow->source = source;
    shadow->flags = zero;
    shadow->unknown2 = zero;
    shadow->unknown4 = zero;
    if (!mode)
        shadow->update = Shadow_UpdateTerrain;
    else
        shadow->update = ActorShadow_UpdateNone;
    sprite = &shadow->sprite;
    ParticleSprite_Initialize(sprite, ((struct TerrainShadowManager *)gShadowManager)->resource,
                              512);
    Effect_SetOrigin(sprite, -8, -8);
    {
        struct ShadowTileResource *resource =
            ((struct TerrainShadowManager *)gShadowManager)->resource;
        s32 width = shadow->sprite.width >> 3, height = shadow->sprite.height >> 3;
        shadow->sprite.tile = width * height * 34 + resource->tileBase;
    }
    shadow->sprite.status = 252;
    shadow->sprite.mode = 2;
    ShadowList_Insert(((struct TerrainShadowManager *)gShadowManager), shadow);
    return 0;
}

u32 Shadow_Destroy(struct TerrainShadow *shadow)
{
    Effect_Remove(&shadow->sprite);
    if (gShadowManager != 0)
        ShadowList_Remove((struct TerrainShadowManager *)gShadowManager, shadow);
    return 0;
}

void ScaledShadowTask_Destroy(void)
{
    gScaledShadowManager = 0;
}
