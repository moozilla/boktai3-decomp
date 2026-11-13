#include "scene_arc.h"

s32 GroundEffect_Link(struct SceneGroundManager *manager, struct SceneGroundEffect *effect)
{
    u32 active = effect->active;
    struct SceneGroundEffect *head;
    if (active != 0)
        return -1;
    effect->prev = (void *)active;
    head = manager->head;
    effect->next = head;
    if (head != 0)
        head->prev = effect;
    manager->head = effect;
    effect->active = 1;
    return 0;
}

s32 GroundEffect_Unlink(struct SceneGroundManager *manager, struct SceneGroundEffect *effect)
{
    struct SceneGroundEffect *prev = effect->prev, *next = effect->next;
    s32 result;
    if (!effect->active)
        return -1;
    if (prev)
        prev->next = next;
    else
        manager->head = next;
    if (next)
        next->prev = prev;
    result = 0;
    effect->active = result;
    return result;
}

void GroundEffect_EnableProjection(struct SceneGroundEffect *effect)
{
    effect->flags &= ~1;
    effect->update = GroundEffect_UpdateProjection;
    effect->update(effect);
}

void GroundEffect_DisableProjection(struct SceneGroundEffect *effect)
{
    effect->flags |= 1;
    effect->update = GroundEffect_IgnoreUpdate;
}

void GroundEffect_IgnoreUpdate(struct SceneGroundEffect *effect)
{
}

s32 Terrain_GetHeightAtFixedPosition(struct ScenePoolVec32 *);

void GroundEffect_UpdateProjection(struct SceneGroundEffect *effect)
{
    struct SceneVec *screen = &effect->screen;
    struct ScenePoolVec32 position = *effect->position;
    position.y = Terrain_GetHeightAtFixedPosition(&position);
    screen->x = (position.x >> 12) - gSceneCamera->x;
    screen->z = position.z >> 12;
    screen->y = position.y;
}

u32 GroundEffect_UpdateManager(struct SceneGroundManager *manager)
{
    struct SceneGroundEffect *effect = manager->head;
    while (effect != 0) {
        effect->update(effect);
        effect = effect->next;
    }
    return 0;
}

u32 ResourceRegistry_UnregisterResource(u32);

u32 GroundEffect_DestroyManager(void)
{
    ResourceRegistry_UnregisterResource(0xF0DE);
    return 0;
}

void ResourceRegistry_RegisterResource(u32, void *, u32);

s32 GroundEffect_InitializeManager(struct SceneGroundManager *manager, void *unused)
{
    ResourceRegistry_RegisterResource(0xf0de, manager, 0);
    manager->head = 0;
    manager->resource = AnimData_FindEntry(0x1c1a);
    return 0;
}

struct SceneGroundManager *GroundEffect_CreateManager(void *argument, u32 unused)
{
    struct SceneGroundManager *manager = Task_Create(9, sizeof(*manager));
    if (manager != 0) {
        Task_SetCallbacks(manager, (void *)GroundEffect_UpdateManager,
                          (void *)GroundEffect_DestroyManager);
        if (GroundEffect_InitializeManager(manager, argument) < 0) {
            Task_Kill(manager);
            return 0;
        }
    }
    return manager;
}

struct SceneGroundManager *ResourceRegistry_GetResource(u32);

void ParticleSprite_Initialize(void *, const void *, u32);

void Effect_SetOrigin(void *, s32, s32);

s32 GroundEffect_Initialize(struct SceneGroundEffect *effect, const struct ScenePoolVec32 *position,
                            u32 kind)
{
    struct SceneGroundManager *manager = ResourceRegistry_GetResource(0xf0de);
    u32 zero;
    void *drawable;
    if (!manager) {
        manager = GroundEffect_CreateManager(0, 0);
        if (!manager)
            return -1;
    }
    zero = 0;
    effect->active = zero;
    effect->position = position;
    effect->kind = kind;
    effect->counter = zero;
    effect->value4 = zero;
    effect->value6 = zero;
    drawable = &effect->flags;
    ParticleSprite_Initialize(drawable, manager->resource, 0x210);
    Effect_SetOrigin(drawable, -8, -8);
    {
        const struct SpriteEffectTileResource *resource = manager->resource;
        effect->tile = (effect->width >> 3) * (effect->height >> 3) * 34 + resource->tileBase;
    }
    effect->priority = 0xfc;
    effect->mode = 2;
    if (!effect->kind)
        effect->update = GroundEffect_UpdateProjection;
    else {
        effect->update = GroundEffect_IgnoreUpdate;
        GroundEffect_DisableProjection(effect);
    }
    GroundEffect_Link(manager, effect);
    return 0;
}

u32 GroundEffect_Destroy(struct SceneGroundEffect *effect)
{
    struct SceneGroundManager *manager = ResourceRegistry_GetResource(0xf0de);
    Effect_Remove(&effect->flags);
    if (manager)
        GroundEffect_Unlink(manager, effect);
    return 0;
}
