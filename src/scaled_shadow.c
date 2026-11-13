#include "scaled_shadow.h"

s32 ScaledShadowManager_AddShadow(struct ScaledShadowManager *manager, struct ScaledShadow *shadow)
{
    u32 active = shadow->active;
    struct ScaledShadow *first;
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

s32 ScaledShadowManager_RemoveShadow(struct ScaledShadowManager *manager,
                                     struct ScaledShadow *shadow)
{
    struct ScaledShadow *previous = shadow->previous;
    struct ScaledShadow *next = shadow->next;
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

void ScaledShadow_Enable(struct ScaledShadow *shadow)
{
    shadow->sprite.flags &= ~1;
    shadow->update = ScaledShadow_Update;
    shadow->update(shadow);
}

void ScaledShadow_Disable(struct ScaledShadow *shadow)
{
    shadow->sprite.flags |= 1;
    shadow->update = ScaledShadow_UpdateNoOp;
}

void ScaledShadow_UpdateNoOp(struct ScaledShadow *shadow)
{
}

void ScaledShadow_Update(struct ScaledShadow *shadow)
{
    u16 index = ShadowTerrain_CellIndex(shadow->source);
    int height;
    shadow->sprite.position.x = shadow->source->x;
    if (shadow->flags & 1)
        shadow->sprite.position.y = shadow->source->y;
    else {
        struct SceneVec *source = shadow->source;
        struct ShadowTerrainCell *terrain = ShadowTerrain_Cell(index);
        int slope = terrain->height >> 4, y = (terrain->height & 15) << 8;
        switch (slope) {
            case 1:
                y -= (u8)source->z;
                break;
            case 2:
                y -= (u8)source->x;
                break;
        }
        shadow->sprite.position.y = y;
    }
    shadow->sprite.position.z = shadow->source->z;
    height = shadow->source->y - shadow->sprite.position.y;
    if (height <= 0)
        shadow->scale = shadow->maxScale;
    else if (height < shadow->minHeight)
        shadow->scale = shadow->maxScale - (height >> shadow->nearShift);
    else if (height > shadow->maxHeight)
        shadow->scale = shadow->minScale;
    else
        shadow->scale = shadow->maxScale - (height >> shadow->shift);
    if (shadow->scale <= 0)
        shadow->scale = 1;
    shadow->sprite.scaleX = shadow->scale;
    shadow->sprite.scaleY = shadow->scale;
    if (shadow->source->y < shadow->sprite.position.y || shadow->sprite.position.y == 0 ||
        ShadowTerrain_HidesShadow(ShadowTerrain_Cell(index)))
        shadow->sprite.flags |= 1;
    else
        shadow->sprite.flags &= ~1;
}

u32 ScaledShadowManager_UpdateAll(struct ScaledShadowManager *manager)
{
    struct ScaledShadow *shadow = manager->head;
    if (shadow) {
        do {
            shadow->update(shadow);
            shadow = shadow->next;
        } while (shadow);
    }
    return 0;
}

void ScaledShadowManager_OnTaskDestroy(void)
{
    gScaledShadowManager = 0;
}

s32 ScaledShadowManager_Init(struct ScaledShadowManager *manager, void *unused)
{
    gScaledShadowManager = (u32)manager;
    manager->head = 0;
    return 0;
}

struct ScaledShadowManager *ScaledShadowManager_Create(void *argument, s32 unused)
{
    struct ScaledShadowManager *manager = Task_Create(9, sizeof(*manager));
    if (manager != 0) {
        Task_SetCallbacks(manager, (void *)ScaledShadowManager_UpdateAll,
                          (void *)ScaledShadowManager_OnTaskDestroy);
        if (ScaledShadowManager_Init(manager, argument) < 0) {
            Task_Kill(manager);
            return 0;
        }
    }
    return manager;
}

s32 ScaledShadow_Initialize(struct ScaledShadow *shadow, struct SceneVec *source, int scale,
                            int minScale, int maxScale, int unknown57, int maxHeight, int minHeight,
                            int shift, int nearShift, int mode)
{
    struct ScaledShadowManager *manager = (void *)gScaledShadowManager;
    struct ShadowAnimationResource *resource;
    struct ScaledShadowSprite *sprite;
    int zero;
    if (!manager) {
        manager = ScaledShadowManager_Create(0, 0);
        if (!manager)
            return -1;
    }
    zero = 0;
    shadow->active = zero;
    shadow->source = source;
    shadow->flags = zero;
    shadow->unknown2 = zero;
    shadow->unknown4 = zero;
    shadow->scale = scale;
    shadow->minScale = minScale;
    shadow->maxScale = maxScale;
    shadow->unknown57 = unknown57;
    shadow->maxHeight = maxHeight;
    shadow->minHeight = minHeight;
    shadow->shift = shift;
    shadow->nearShift = nearShift;
    if (!mode)
        shadow->update = ScaledShadow_Update;
    else
        shadow->update = ScaledShadow_UpdateNoOp;
    resource = &shadow->resource;
    AnimData_Load(resource, 0x1C1B);
    sprite = &shadow->sprite;
    Sprite_Init(sprite, resource, 514);
    sprite->frame = 5;
    shadow->sprite.priority = 2;
    ScaledShadowManager_AddShadow(manager, shadow);
    return 0;
}

u32 ScaledShadow_InitializeDefault(struct ScaledShadow *shadow, struct SceneVec *source)
{
    return ScaledShadow_Initialize(shadow, source, 0x40, 0x10, 0x40, 0, 0xa00, 1, 5, 1, 0);
}

u32 ScaledShadow_InitializeDefaultSmall(struct ScaledShadow *shadow, struct SceneVec *source)
{
    u32 result = ScaledShadow_Initialize(shadow, source, 0x40, 0x10, 0x40, 0, 0xa00, 1, 5, 1, 0);
    if (result == 0)
        ScaledShadow_SetGraphic(shadow, 0x1c1b, 6);
    return result;
}

u32 ScaledShadow_InitializeDefaultLarge(struct ScaledShadow *shadow, struct SceneVec *source)
{
    u32 result = ScaledShadow_Initialize(shadow, source, 0x40, 0x10, 0x40, 0, 0xa00, 1, 5, 1, 0);
    if (result == 0)
        ScaledShadow_SetGraphic(shadow, 0x1c1b, 7);
    return result;
}

u32 ScaledShadow_InitializeRange(struct ScaledShadow *shadow, struct SceneVec *source, s32 minScale,
                                 s32 maxScale)
{
    return ScaledShadow_Initialize(shadow, source, maxScale, minScale, maxScale, 0, 0xa00, 1, 5, 1,
                                   0);
}

u32 ScaledShadow_InitializeSmallRange(struct ScaledShadow *shadow, struct SceneVec *source,
                                      s32 minScale, s32 maxScale)
{
    u32 result =
        ScaledShadow_Initialize(shadow, source, maxScale, minScale, maxScale, 0, 0xa00, 1, 5, 1, 0);
    if (result == 0)
        ScaledShadow_SetGraphic(shadow, 0x1c1b, 6);
    return result;
}

u32 ScaledShadow_InitializeLargeRange(struct ScaledShadow *shadow, struct SceneVec *source,
                                      s32 minScale, s32 maxScale)
{
    u32 result =
        ScaledShadow_Initialize(shadow, source, maxScale, minScale, maxScale, 0, 0xa00, 1, 5, 1, 0);
    if (result == 0)
        ScaledShadow_SetGraphic(shadow, 0x1c1b, 7);
    return result;
}

u32 ScaledShadow_Destroy(struct ScaledShadow *shadow)
{
    Sprite_Remove(&shadow->sprite);
    if (gScaledShadowManager != 0)
        ScaledShadowManager_RemoveShadow((void *)gScaledShadowManager, shadow);
    return 0;
}

u32 ScaledShadow_SetGraphic(struct ScaledShadow *shadow, u32 resourceId, u32 frame)
{
    struct ShadowAnimationResource *resource = &shadow->resource;
    struct ScaledShadowSprite *sprite;
    AnimData_Load(resource, resourceId);
    sprite = &shadow->sprite;
    Sprite_ApplyAnimation(sprite, resource);
    sprite->frame = frame;
}

u32 ScaledShadow_SetFlag(struct ScaledShadow *shadow, u32 on)
{
    if (on)
        shadow->sprite.flags |= 2;
    else
        shadow->sprite.flags &= ~2;
}

void ScaledShadow_SetScaleRange(struct ScaledShadow *shadow, s32 scale, s32 minScale, s32 maxScale,
                                s32 unknown57, s32 maxHeight, s32 minHeight, s32 shift,
                                s32 nearShift)
{
    shadow->scale = scale;
    shadow->minScale = minScale;
    shadow->maxScale = maxScale;
    shadow->unknown57 = unknown57;
    shadow->maxHeight = maxHeight;
    shadow->minHeight = minHeight;
    shadow->shift = shift;
    shadow->nearShift = nearShift;
}
