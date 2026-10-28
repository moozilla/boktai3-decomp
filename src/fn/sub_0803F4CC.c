#include "scene_arc.h"
struct SceneArcManager *ResourceRegistry_GetResource(u32);
struct SceneArcSlot *SceneArc_FindFreeSlot(struct SceneArcManager *, s32 *);
struct SceneArcManager *SceneArc_CreateManager(void);
void GroundEffect_EnableProjection(struct SceneGroundEffect *);
s32 SceneArc_Create(const struct ScenePoolVec32 *from, const struct ScenePoolVec32 *to,
                    u32 flightDuration, u32 endFrame, s32 height)
{
    const struct ScenePoolVec32 *startArg = from;
    const struct ScenePoolVec32 *targetArg = to;
    u32 duration = flightDuration;
    u32 end = endFrame;
    struct SceneArcManager *manager = ResourceRegistry_GetResource(0xfe2c);
    struct SceneArcSlot *slot;
    s32 index;
    u32 zero;
    struct SceneArcSprite *sprite;
    if (!manager) {
        SceneArc_CreateManager();
        return -1;
    }
    slot = SceneArc_FindFreeSlot(manager, &index);
    if (!slot)
        return -1;
    sprite = &slot->sprite;
    {
        u32 firstZero = 0;
        zero = firstZero;
        slot->frame = firstZero;
    }
    slot->flightDuration = duration;
    slot->endFrame = end;
    slot->releaseFrame = end + 8;
    slot->height = height;
    slot->start = *startArg;
    slot->position = *startArg;
    slot->target = *targetArg;
    slot->grounded = zero;
    {
        struct ScenePoolVec32 *position = &slot->position;
        struct SceneVec *screen = &slot->sprite.screen;
        screen->x = (slot->position.x >> 12) - gSceneCamera->x;
        {
            s32 y = position->y;
            screen->y = ((position->z + y) >> 12) + 96;
        }
        screen->z = position->z >> 12;
    }
    {
        u32 frameZero = zero;
        struct SceneArcSprite *loadSprite = sprite;
        u32 flags;
        loadSprite->frame = frameZero;
        flags = loadSprite->flags & ~1;
        {
            struct SceneArcSprite *storeSprite = sprite;
            storeSprite->flags = flags;
        }
    }
    {
        void *animation = &slot->animation;
        void *resource = manager->animationResource;
        u32 callZero;
        callZero = zero;
        Anim_SetSequence(animation, resource, 2, 0, callZero);
    }
    GroundEffect_EnableProjection(&slot->ground);
    manager->activeMask |= 1 << index;
    return 0;
}
