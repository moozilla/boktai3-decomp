#include "scene_actor_lifecycle.h"

void *Actor_FindResourceData(int type, int variant)
{
    int i = 0;
    const struct SceneActorResourceEntry *entry = gUnk_086053D4;
    for (; i < 32; entry++, i++)
        if (entry->type == type && entry->variant == variant)
            return entry->payload.data;
    return 0;
}

const struct SceneActorResourceEntry *Actor_FindResourceEntry(int type, int variant)
{
    int i = 0;
    const struct SceneActorResourceEntry *entry = gUnk_086054D4;
    for (; i < 32; entry++, i++)
        if (entry->type == type && entry->variant == variant)
            return &gUnk_086054D4[i];
    return 0;
}

int Actor_InitResources(void *unused, struct SceneActor *actor, int type, int variant)
{
    actor->animationEntries = Actor_FindResourceData(type, variant);
    if (actor->animationEntries) {
        actor->resourceEntry = Actor_FindResourceEntry(type, variant);
        if (actor->resourceEntry)
            return 0;
    }
    return -1;
}

void Actor_LinkToManager(struct SceneActorManager *manager, struct SceneActor *actor)
{
    if (manager->head)
        manager->head->next = actor;
    actor->next = 0;
    actor->previous = manager->head;
    manager->head = actor;
    actor->active = 1;
}

void Actor_UnlinkFromManager(struct SceneActorManager *manager, struct SceneActor *actor)
{
    if (actor->next)
        actor->next->previous = actor->previous;
    else
        manager->head = actor->previous;
    if (actor->previous)
        actor->previous->next = actor->next;
}

struct SceneActor *Actor_FindById(struct SceneActorManager *manager, int id)
{
    struct SceneActor *actor = manager->head;
    while (actor) {
        if (actor->body.id == id)
            return actor;
        actor = actor->previous;
    }
    return 0;
}

struct SceneActor *Actor_Allocate(int unused, int variant)
{
    if (gSceneActorListManager) {
        int size = sizeof(struct SceneActor);
        struct SceneActor *actor = Heap_Alloc(size);
        if (actor) {
            Memory_Clear(actor, size);
            actor->variant = variant;
            return actor;
        }
    }
    return 0;
}

int Actor_InitMotion(struct SceneActor *actor, int id, const struct ContactPosition *position,
                     int direction)
{
    struct SceneActorBody *body;
    int result;
    actor->direction = direction;
    body = &actor->body;
    SceneBody_Init(body, id, position, (u8)direction, 7, actor);
    result = SceneBody_SetPositionTarget(body, &actor->positionLink);
    {
        u32 mask = 4;
        body->flags |= mask;
    }
    return result;
}

int Actor_InitDrawable(struct SceneActor *actor, int kind, int resourceId, int sequenceId,
                       int flags)
{
    void *drawable = &actor->drawable.common;
    int status = -1;
    int result;
    if (kind == 2) {
        struct DisplayKeyframeState *keyframes;
        status = DisplayAnimation_InitializeKeyframes(
            drawable, resourceId, flags, (const struct DisplayPosition *)&actor->body.position);
        keyframes = &actor->drawable.keyframe.keyframes;
        actor->defaultPaletteBank = keyframes->paletteIndex;
        SceneBody_SetKeyframeDisplay(&actor->body, keyframes);
    } else if (kind == 1) {
        struct SceneActorSequenceResource *part;
        status = DisplayAnimation_InitializeSequence(
            drawable, resourceId, sequenceId, flags,
            (const struct DisplayPosition *)&actor->body.position);
        part = &actor->drawable.sequence.resource;
        actor->defaultPaletteBank = part->paletteIndex;
        SceneBody_SetSprite(&actor->body, &actor->drawable.sequence.sprite);
    }
    actor->paletteBank = actor->defaultPaletteBank;
    result = DisplayAnimation_SetPriority(drawable, 2);
    if (status < 0)
        return -1;
    return result;
}

int Actor_InitShadow(struct SceneActor *actor, int resourceId, int shadowKind, int graphicId,
                     int size)
{
    actor->shadowKind = shadowKind;
    {
        unsigned shifted = ((unsigned)(*(&shadowKind))) << 24;
        shadowKind = shifted >> 24;
    }
    if (shadowKind == 1) {
        void *shadow = &actor->shadow;
        Shadow_Initialize(shadow, resourceId, 1, size);
        ActorShadow_Enable(shadow);
    } else if (shadowKind == 2) {
        void *shadow = &actor->shadow;
        ScaledShadow_Initialize(shadow, resourceId, size, size, size, size, 2000, 400, 2, 4, 1);
        ScaledShadow_SetGraphic(shadow, 0x1c1b, graphicId);
        ScaledShadow_Enable(shadow);
    }
    return 0;
}

u32 Actor_DestroyShadow(struct SceneActor *actor)
{
    u8 shadowKind = actor->shadowKind;
    if (shadowKind == 1)
        Shadow_Destroy(&actor->shadow);
    else if (shadowKind == 2)
        ScaledShadow_Destroy(&actor->shadow);
    return 0;
}

u32 Actor_DisableShadow(struct SceneActor *actor)
{
    u8 shadowKind = actor->shadowKind;
    if (shadowKind == 1)
        ActorShadow_Disable(&actor->shadow);
    else if (shadowKind == 2)
        ScaledShadow_Disable(&actor->shadow);
    return 0;
}

u32 Actor_EnableShadow(struct SceneActor *actor)
{
    u8 shadowKind = actor->shadowKind;
    if (shadowKind == 1)
        ActorShadow_Enable(&actor->shadow);
    else if (shadowKind == 2)
        ScaledShadow_Enable(&actor->shadow);
    return 0;
}

int Actor_RegisterMotion(struct SceneActor *actor)
{
    int result;
    if (TerrainOverlay_QueueAddActor(&actor->body) >= 0) {
        actor->motionRegistered = 1;
        result = 0;
    } else
        result = -1;
    return result;
}

int Actor_UnregisterMotion(struct SceneActor *actor)
{
    int result;
    if (actor->motionRegistered) {
        TerrainOverlay_QueueRemoveActor(&actor->body);
        actor->motionRegistered = 0;
        result = 0;
    } else
        result = -1;
    return result;
}

u32 Actor_SetPaletteBank(struct SceneActor *actor, u32 paletteBank)
{
    struct DisplayAnimation *drawable = &actor->drawable.common;
    actor->paletteBank = paletteBank;
    return DisplayAnimation_SelectPaletteBank(drawable, actor->paletteBank);
}

u32 Actor_SetDefaultPaletteBank(struct SceneActor *actor, u16 paletteBank)
{
    actor->defaultPaletteBank = paletteBank;
    return Actor_SetPaletteBank(actor, actor->defaultPaletteBank);
}

s32 Actor_ResetPaletteBank(struct SceneActor *actor)
{
    return Actor_SetPaletteBank(actor, actor->defaultPaletteBank);
}

int Actor_Destroy(struct SceneActor *actor)
{
    int result;
    if (gSceneActorListManager && actor->active) {
        Actor_UnlinkFromManager(gSceneActorListManager, actor);
        Actor_UnregisterMotion(actor);
        SceneBody_Unregister(&actor->body);
        DisplayAnimation_Destroy(&actor->drawable.common);
        Actor_DestroyShadow(actor);
        actor->active = 0;
        Heap_Free(actor);
        result = 0;
    } else
        result = -1;
    return result;
}

int Actor_RunCallbacks(void *context, struct SceneActor *actor)
{
    int callbackIndex;
    void (**callback)(void *, struct SceneActor *);
    if (!actor->active)
        return -1;
    callbackIndex = 0;
    callback = actor->callbacks;
    for (;;) {
        if (*callback)
            (*callback)(context, actor);
        callback++;
        callbackIndex++;
        if (callbackIndex >= 4)
            return 0;
    }
}
