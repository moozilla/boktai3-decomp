#include "sprite_effect_playback.h"
#include "scene_actor_terrain.h"

// confidence=low: animation controller with terrain bounce and intermittent gravity.
#define SpriteEffectController_UpdateBouncing sub_0806AB1C
void Heap_Free(void *);
static inline struct ContactPosition *AddVelocity(struct ContactPosition *position,
                                                  struct ContactPosition *velocity)
{
    position->x = velocity->x + position->x;
    position->y = velocity->y + position->y;
    position->z = velocity->z + position->z;
    return velocity;
}

static inline s32 Square(s32 value)
{
    return value * value;
}

static inline s32 CellAndVelocity(struct ContactPosition *position,
                                  struct ContactPosition *inputVelocity,
                                  struct ContactPosition **outputVelocity)
{
    s32 x = position->x >> 8;
    s32 z = position->z >> 8;
    *outputVelocity = inputVelocity;
    if (x < 0 || z < 0 || (u32)x >= gTerrainWidth || (u32)z >= gTerrainHeight)
        return 0;
    return gTerrainMap->rows[z] + x;
}

static inline u32 HasMotion(u16 *flags)
{
    return *flags & 0x80;
}

void SpriteEffectController_UpdateBouncing(struct SpriteEffectControllerPool *unusedPool,
                                           struct SpriteEffectController *controller)
{
    struct ContactPosition *position;
    u16 *flags;
    u32 ended;
    u32 mode;
    if (controller->enter) {
        if (controller->flags & 8)
            ((struct SpriteEffectSequenceDrawable *)controller->drawable)->flags &= ~1;
        else
            ((struct SpriteEffectTileDrawable *)controller->drawable)->flags &= ~1;
        controller->frame = 0;
        controller->enter = 0;
    }
    {
        u16 *lowFlags = &controller->flags;
        mode = *lowFlags & 8;
        flags = lowFlags;
    }
    if (mode) {
        ended = SpriteEffectController_StepDrawable(controller, 1);
        position = &((struct SpriteEffectSequenceDrawable *)controller->drawable)->position;
    } else {
        ended = SpriteEffectController_StepDrawable(controller, 0);
        position = &((struct SpriteEffectTileDrawable *)controller->drawable)->position;
    }
    SpriteEffectController_RefreshCollider(controller, position);
    if (HasMotion(flags)) {
        struct ContactPosition *velocity;
        s32 cell;
        s32 height;
        cell = CellAndVelocity(position, AddVelocity(position, &controller->velocity), &velocity);
        height = SceneActorTerrain_GetHeight(cell, position);
        if (position->y <= height) {
            s32 y;
            s32 sum;
            position->y = height;
            controller->velocity.x = Div(controller->velocity.x * 5, 10);
            y = controller->velocity.y;
            if (y < 0)
                y = -y;
            controller->velocity.y = Div(y * 2, 10);
            controller->velocity.z = Div(controller->velocity.z * 5, 10);
            sum = Square(controller->velocity.x) + Square(velocity->y);
            if (sum + Square(velocity->z) == 0)
                *flags &= 0xFF7F;
        } else if (Mod(controller->frame, 4))
            controller->velocity.y -= 2;
    }
    controller->frame++;
    if (ended) {
        if (controller->lifetime)
            goto tickTimer;
        if (*flags & 1) {
            struct SpriteEffectDrawablePrefix *drawable;
            gSpriteEffectControllerPool->used &= ~(1 << controller->index);
            controller->state = 0;
            {
                u16 *reloadFlags = flags;
                mode = *reloadFlags & 8;
            }
            if (mode) {
                drawable = controller->drawable;
                if (drawable->active) {
                    Sprite_Remove(drawable);
                    Heap_Free(controller->drawable);
                }
            } else {
                drawable = controller->drawable;
                if (drawable->active) {
                    Effect_Remove(drawable);
                    Heap_Free(controller->drawable);
                }
            }
            controller->index = -1;
            *flags = 0;
        } else
            controller->state = 4;
        controller->enter = 1;
    } else if (controller->lifetime) {
    tickTimer:
        controller->lifetime--;
    }
}
