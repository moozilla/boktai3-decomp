#include "sprite_effect_controller.h"
#include "scene_actor_terrain.h"

// confidence=low: animation controller with terrain bounce and intermittent gravity.
#define SpriteEffectController_UpdateTileBounce sub_0806AF74
void Heap_Free(void *);
#ifdef NONMATCHING
// fuzzy: 99.8%
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

static inline u32
SpriteEffectController_GetTileFrame(u32 frame, u32 width, u32 height,
                                    const struct SpriteEffectTileResource *graphics)
{
    height *= width;
    height *= frame;
    height += graphics->tileBase;
    return height;
}
void SpriteEffectController_UpdateTileBounce(struct SpriteEffectControllerPool *unusedPool,
                                             struct SpriteEffectController *controller)
{
    struct SpriteEffectTileDrawable *tile = controller->drawable;
    struct ContactPosition *position;
    u16 *flags;
    u32 ended;
    u32 mode;
    if (controller->enter) {
        tile->flags &= ~1;
        controller->frame = 0;
        controller->enter = 0;
    }
    mode = controller->flags & 0x80;
    flags = &controller->flags;
    position = &tile->position;
    if (mode) {
        struct ContactPosition *velocity;
        s32 cell;
        s32 height;
        cell = CellAndVelocity(&tile->position, AddVelocity(&tile->position, &controller->velocity),
                               &velocity);
        height = ((*SceneActorTerrain_GetCell(cell)) & 15) << 8;
        if (tile->position.y <= height) {
            s32 y;
            s32 sum;
            tile->position.y = height;
            controller->velocity.x = Div(controller->velocity.x * 5, 10);
            y = controller->velocity.y;
            if (y < 0)
                y = -y;
            controller->velocity.y = Div(y * 2, 10);
            controller->velocity.z = Div(controller->velocity.z * 5, 10);
            sum = Square(controller->velocity.x) + Square(velocity->y);
            if ((sum + Square(velocity->z)) == 0)
                *flags &= 0xFF7F;
        } else if (Mod(controller->frame, 4))
            controller->velocity.y -= 2;
    }
    SpriteEffectController_RefreshCollider(controller, &tile->position);
    {
        u8 animationEnded;

        void *const *prefix = (void *const *)controller;
        void *raw = *(prefix++);
        struct DisplaySequenceState *state = (struct DisplaySequenceState *)prefix;
        const struct SpriteEffectTileResource *graphics;
        const volatile u16 *command;
        u32 duration;
        u32 flags;
        graphics = ((struct SpriteEffectTileDrawable *)raw)->graphics;
        command = state->commands + state->index;
        {
            u32 frame = (*command) >> 6;
            u32 width = tile->width >> 3;
            u32 height = tile->height >> 3;
            height = SpriteEffectController_GetTileFrame(frame, width, height, graphics);
            tile->tile = height;
        }
        {
            union DisplaySequenceControl control;
            u32 bit, first;
            control.raw = state->flags;
            bit = 1;
            first = (u8)(control.raw & bit);
            if (first != (((*command & 0x30) >> 4) & bit))
                tile->flags |= 4;
            else
                tile->flags &= ~4;
        }
        {
            union DisplaySequenceControl control;
            u32 bit, first;
            control.raw = state->flags;
            bit = 2;
            first = (u8)(control.raw & bit);
            if (first != (((*command & 0x30) >> 4) & bit))
                tile->flags |= 8;
            else
                tile->flags &= ~8;
        }
        if (++state->elapsed >= state->duration) {
            state->elapsed = 0;
            flags = state->flags;
            if (flags & 4) {
                if (state->index == 0) {
                    u32 hold = 8;
                    hold &= flags;
                    if (hold)
                        state->elapsed = state->duration;
                    else
                        state->index = state->frameCount - 1;
                    animationEnded = 1;
                } else {
                    state->index--;
                    animationEnded = 0;
                }
            } else {
                if (++state->index >= state->frameCount) {
                    u32 hold = 8;
                    hold &= flags;
                    if ((u8)hold) {
                        state->elapsed = state->duration;
                        state->index = state->frameCount - 1;
                    } else {
                        state->index = 0;
                    }
                    animationEnded = 1;
                } else {
                    animationEnded = 0;
                }
            }
            command = state->commands + state->index;
            state->duration = ((*command & 15) * (s32)state->speed) >> 6;
        } else {
            animationEnded = 0;
        }
        ended = animationEnded;
    }
    if (controller->lifetime) {
        if ((controller->sequence.index == 5) &&
            (controller->sequence.elapsed == (controller->sequence.duration - 1))) {
            controller->sequence.index = 0;
            controller->sequence.elapsed = 0;
        }
        controller->lifetime--;
    }
    controller->frame++;
    if (ended) {
        if ((*flags) & 1) {
            struct SpriteEffectDrawablePrefix *drawable;
            gSpriteEffectControllerPool->used &= ~(1 << controller->index);
            controller->state = 0;
            {
                u16 *reloadFlags = flags;
                mode = (*reloadFlags) & 8;
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
    }
}

#else
INCLUDE_ASM("asm/nonmatching", sub_0806AF74);
#endif
