// MATCHING_CONCESSION: register-pinning
#include "scene_arc.h"
struct SceneArcManager *sub_0821A2C4(u32);
struct SceneArcSlot *SceneArc_FindFreeSlot(struct SceneArcManager *, s32 *);
struct SceneArcManager *SceneArc_CreateManager(void);
void GroundEffect_EnableProjection(struct SceneGroundEffect *);
s32 SceneArc_Create(const struct ScenePoolVec32 *from, const struct ScenePoolVec32 *to,
                    u32 flightDuration, u32 endFrame, s32 height)
{
    const struct ScenePoolVec32 *startArg = from;
    const struct ScenePoolVec32 *targetArg = to;
    u32 duration = flightDuration;
    register u32 end asm("r5") = endFrame;
    struct SceneArcManager *manager = sub_0821A2C4(0xfe2c);
    struct SceneArcSlot *slot;
    s32 index;
    register u32 zero asm("r12");
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
        register u32 firstZero asm("r1") = 0;
        zero = firstZero;
        slot->frame = firstZero;
    }
    slot->flightDuration = duration;
    slot->endFrame = end;
    slot->releaseFrame = end + 8;
    asm("" : : "r"(end));
    slot->height = height;
    slot->start = *startArg;
    slot->position = *startArg;
    slot->target = *targetArg;
    slot->grounded = zero;
    {
        struct ScenePoolVec32 *position = &slot->position;
        struct SceneVec *screen = &slot->sprite.screen;
        screen->x = (slot->position.x >> 12) - gUnk_020004B8->x;
        {
            s32 y = position->y;
            screen->y = ((position->z + y) >> 12) + 96;
        }
        screen->z = position->z >> 12;
    }
    {
        register u32 frameZero asm("r2") = zero;
        register struct SceneArcSprite *loadSprite asm("r1") = sprite;
        register u32 flags asm("r0");
        loadSprite->frame = frameZero;
        flags = loadSprite->flags & ~1;
        asm("" : "+m"(sprite));
        {
            register struct SceneArcSprite *storeSprite asm("r3") = sprite;
            storeSprite->flags = flags;
        }
    }
    {
        void *animation = &slot->animation;
        void *resource = manager->animationResource;
        register u32 callZero asm("r4");
        callZero = zero;
        Anim_SetSequence(animation, resource, 2, 0, callZero);
    }
    GroundEffect_EnableProjection(&slot->ground);
    manager->activeMask |= 1 << index;
    return 0;
}
