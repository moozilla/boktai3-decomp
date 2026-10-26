// MATCHING_CONCESSION: register-pinning
#include "sprite_effect_playback.h"
#include "scene_actor_terrain.h"

// confidence=low: animation controller with terrain bounce and intermittent gravity.
#define SpriteEffectController_UpdateTileBounce sub_0806AF74
void Heap_Free(void *);
#ifdef NONMATCHING
// fuzzy: 92.7%
// fix.py best (sweep after the inlined-insn fix)
// Frame-address hoisting and reverse-playback pointer copies still differ.
static inline u32 StepTile(struct SpriteEffectController *controller, u32 sequenceMode)
{
    void *const *prefix = (void *const *)controller;
    void *raw = *prefix++;
    struct DisplaySequenceState *state = (struct DisplaySequenceState *)prefix;
    struct SpriteEffectDrawablePrefix *drawable = raw;
    const struct SpriteEffectTileResource *graphics;
    const volatile u16 *command;
    u32 ended, duration, flags;
    if (!sequenceMode)
        graphics = ((struct SpriteEffectTileDrawable *)raw)->graphics;
    command = state->commands + state->index;
    if (sequenceMode)
        ((struct SpriteEffectSequenceDrawable *)raw)->frame = *command >> 6;
    else {
        struct SpriteEffectTileDrawable *tile = raw;
        u32 frame = *command >> 6;
        u32 width = tile->width >> 3;
        register u32 height asm("r0") = tile->height >> 3;
        height *= width;
        height *= frame;
        height += graphics->tileBase;
        tile->tile = height;
    }
    {
        u32 input = state->flags;
        u32 bit = 1;
        register u32 first asm("r1");
        asm("" : "+r"(bit));
        first = bit;
        first &= input;
        if (first != (((*command & 0x30) >> 4) & bit))
            drawable->flags |= 4;
        else
            drawable->flags &= ~4;
    }
    {
        u32 input = state->flags;
        u32 bit = 2;
        register u32 first asm("r1");
        asm("" : "+r"(bit));
        first = bit;
        first &= input;
        asm("" : "+r"(first));
        first = (u8)first;
        if (first != (((*command & 0x30) >> 4) & bit))
            drawable->flags |= 8;
        else
            drawable->flags &= ~8;
    }
    {
        u32 elapsed = state->elapsed + 1;
        u32 mask;
        register u32 savedDuration asm("r3");
        state->elapsed = elapsed;
        if (sequenceMode) {
            register u32 temp asm("r1") = 0xFFFF;
            asm("" : "+r"(temp));
            mask = temp;
            asm("" : "+&r"(mask) : "r"(temp));
        } else {
            register u32 temp asm("r2") = 0xFFFF;
            asm("" : "+r"(temp));
            mask = temp;
            asm("" : "+&r"(mask) : "r"(temp));
        }
        duration = state->duration;
        elapsed = (u16)elapsed;
        savedDuration = duration;
        if (elapsed >= duration) {
            state->elapsed = 0;
            flags = state->flags;
            if (flags & 4) {
                u32 index;
                asm("");
                index = state->index;
                if (!index) {
                    u32 hold = flags & 8;
                    asm("" : "+r"(hold));
                    if (hold)
                        state->elapsed = savedDuration;
                    else {
                        state->index = state->frameCount - 1;
                        asm("");
                    }
                    ended = 1;
                } else {
                    state->index = index - 1;
                    ended = 0;
                }
            } else {
                u32 index = state->index + 1;
                u32 frameCount;
                state->index = index;
                frameCount = state->frameCount;
                if ((index & mask) >= frameCount) {
                    u32 hold = flags & 8;
                    asm("" : "+r"(hold));
                    if ((u8)hold) {
                        state->elapsed = duration;
                        state->index = frameCount - 1;
                    } else
                        state->index = 0;
                    ended = 1;
                } else
                    ended = 0;
            }
            command = state->commands + state->index;
            if (sequenceMode) {
                s32 delay;
                state->frameWeight = *command & 15;
                {
                    s32 weight = state->frameWeight;
                    s32 speed = state->speed;
                    delay = (speed * weight) >> 6;
                }
                state->duration = delay;
                if (!(u8)delay)
                    state->duration = 1;
            } else
                state->duration = ((*command & 15) * (s32)state->speed) >> 6;
        } else
            ended = 0;
    }
    return ended;
}

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

inline static u32 StepTile_0806AF74(struct SpriteEffectController *controller, u32 sequenceMode)
{
    void *const *prefix = (void *const *)controller;
    void *raw = *(prefix++);
    struct DisplaySequenceState *state = (struct DisplaySequenceState *)prefix;
    struct SpriteEffectDrawablePrefix *drawable = raw;
    const struct SpriteEffectTileResource *graphics;
    const volatile u16 *command;
    u32 ended;
    u32 duration;
    u32 flags;
    if (!sequenceMode)
        graphics = ((struct SpriteEffectTileDrawable *)raw)->graphics;
    command = state->commands + state->index;
    if (sequenceMode)
        ((struct SpriteEffectSequenceDrawable *)raw)->frame = (*command) >> 6;
    else {
        struct SpriteEffectTileDrawable *tile = raw;
        u32 frame = (*command) >> 6;
        u32 width = tile->width >> 3;
        register u32 height asm("r0") = tile->height >> 3;
        height *= width;
        height *= frame;
        height += graphics->tileBase;
        tile->tile = height;
    }
    {
        u32 input = state->flags;
        u32 bit = 1;
        register u32 first asm("r1");
        asm("" : "+r"(bit));
        first = bit;
        first &= input;
        if (first != ((((*command) & 0x30) >> 4) & bit))
            drawable->flags |= 4;
        else
            drawable->flags &= ~4;
    }
    {
        u32 input = state->flags;
        u32 bit = 2;
        register u32 first asm("r1");
        asm("" : "+r"(bit));
        first = bit;
        first &= input;
        asm("" : "+r"(first));
        first = (u8)first;
        if (first != ((((*command) & 0x30) >> 4) & bit))
            drawable->flags |= 8;
        else
            drawable->flags &= ~8;
    }
    {
        u32 elapsed = state->elapsed + 1;
        u32 mask;
        register u32 savedDuration asm("r3");
        state->elapsed = elapsed;
        if (sequenceMode) {
            register u32 temp asm("r1") = 0xFFFF;
            asm("" : "+r"(temp));
            mask = temp;
            asm("" : "+&r"(mask) : "r"(temp));
        } else {
            register u32 temp asm("r2") = 0xFFFF;
            asm("" : "+r"(temp));
            mask = temp;
            asm("" : "+&r"(mask) : "r"(temp));
        }
        duration = state->duration;
        elapsed = (u16)elapsed;
        savedDuration = duration;
        if (elapsed >= duration) {
            state->elapsed = 0;
            flags = state->flags;
            if (flags & 4) {
                s16 index;
                asm("");
                index = state->index;
                if (!index) {
                    u32 hold = flags & 8;
                    asm("" : "+r"(hold));
                    if (hold)
                        state->elapsed = savedDuration;
                    else {
                        state->index = state->frameCount - 1;
                        asm("");
                    }
                    ended = 1;
                } else {
                    state->index = index - 1;
                    ended = 0;
                }
            } else {
                u32 index = state->index + 1;
                u32 frameCount;
                state->index = index;
                frameCount = state->frameCount;
                if ((index & mask) >= frameCount) {
                    u32 hold = flags & 8;
                    asm("" : "+r"(hold));
                    if ((u8)hold) {
                        state->elapsed = duration;
                        state->index = frameCount - 1;
                    } else
                        state->index = 0;
                    ended = 1;
                } else
                    ended = 0;
            }
            command = state->commands + state->index;
            if (sequenceMode) {
                s32 delay;
                state->frameWeight = (*command) & 15;
                {
                    s32 weight = state->frameWeight;
                    s32 speed = state->speed;
                    delay = (speed * weight) >> 6;
                }
                state->duration = delay;
                if (!((u8)delay))
                    state->duration = 1;
            } else
                state->duration = (((*command) & 15) * ((s32)state->speed)) >> 6;
        } else
            ended = 0;
    }
    return ended;
}

void SpriteEffectController_UpdateTileBounce(struct SpriteEffectControllerPool *unusedPool,
                                             struct SpriteEffectController *controller)
{
    struct SpriteEffectTileDrawable *drawable = controller->drawable;
    struct ContactPosition *position;
    u16 *flags;
    u32 ended;
    u32 mode;
    if (controller->enter) {
        drawable->flags &= ~1;
        controller->frame = 0;
        controller->enter = 0;
    }
    {
        register u16 *lowFlags asm("r2") = &controller->flags;
        register u32 loaded asm("r1") = *lowFlags;
        register u32 selected asm("r0") = loaded & 0x80;
        asm("" : "+r"(selected) : "r"(lowFlags));
        flags = lowFlags;
        mode = selected;
    }
    position = &drawable->position;
    if (mode) {
        struct ContactPosition *velocity;
        s32 cell;
        s32 height;
        cell = CellAndVelocity(&drawable->position,
                               AddVelocity(&drawable->position, &controller->velocity), &velocity);
        height = ((*SceneActorTerrain_GetCell(cell)) & 15) << 8;
        if (drawable->position.y <= height) {
            s32 y;
            s32 sum;
            drawable->position.y = height;
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
    SpriteEffectController_RefreshCollider(controller, &drawable->position);
    ended = StepTile_0806AF74(controller, 0);
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
            gUnk_02000178->used &= ~(1 << controller->index);
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
