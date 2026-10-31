#include "global.h"
#include "engine_globals.h"
#include "display_sequence.h"
#include "scene_vec.h"

struct Sprite {
    u32 flags;
    u8 visible, unknown5, palette, unknown7;
    s8 scaleX, scaleY;
    u8 padA[6];
    u16 frame;
    u8 pad12[10];
    struct SceneVec position;
    u8 pad24[0x24];
};
struct Disc {
    u32 flags;
    u8 visible;
    u8 unknown5[3];
    s8 scaleX, scaleY;
    u8 padA[6];
    u16 frame;
    u8 pad12[10];
    struct SceneVec position;
    u8 pad24[8];
    struct DisplaySequenceState anim;
    u8 pad3C[12];
    struct SceneVec projected;
    u8 pad50[0x34];
    struct SceneVec target, velocity;
    u8 pad94[8];
    u8 collider[0x54];
    u16 falling;
    u8 state;
    s8 slot;
    u16 unknownF4, age;
    u8 entered;
};
struct DiscPool {
    u8 pad[0x38];
    u32 allocated;
};
struct Map {
    u32 unknown;
    u8 *cells;
    u8 pad8[0x1c];
    u16 rows[1];
};
u8 *TerrainModifiers_FindCellOverride(s32, s32);
void Sprite_Remove(void *);
void Sound_PlaySong(u32);

s32 Math_Atan2(s32, s32);
extern const s16 gUnk_086149C4[];
void Collider_SetPosition(void *, void *, u32);
void Collider_Refresh(void *);

static inline u16 Height(struct SceneVec *position)
{
    s32 x = position->x >> 8, z = position->z >> 8, index, height, slope;
    u8 *cell;
    if (x < 0 || z < 0 || (u32)x >= gTerrainWidth || (u32)z >= gTerrainHeight)
        index = 0;
    else
        index = ((struct Map *)gTerrainMap)->rows[z] + x;
    cell = TerrainModifiers_FindCellOverride(index, 1);
    if (cell)
        cell += 4;
    else
        cell = ((struct Map *)gTerrainMap)->cells + (index * 4 + 12);
    slope = *cell >> 4;
    height = (*cell & 15) << 8;
    switch (slope) {
        case 1:
            height -= (u8)position->z;
            break;
        case 2:
            height -= (u8)position->x;
            break;
    }
    return height;
}

static inline s32 Scale(s32 value)
{
    return value >= 0 ? value >> 12 : -((-value) >> 12);
}

static inline void Polar(s32 angle, s32 radius, struct SceneVec *out)
{
    out->x = Scale(gUnk_086149C4[(angle + 64) & 255] * radius);
    out->y = 0;
    out->z = Scale(gUnk_086149C4[angle & 255] * radius);
}

static inline u8 TestFalling(u32 mask, u16 *flags)
{
    if (*flags & mask)
        return 1;
    return 0;
}

static inline void Add(struct SceneVec *out, struct SceneVec *a, struct SceneVec *b)
{
    out->x = b->x + a->x;
    out->y = b->y + a->y;
    out->z = b->z + a->z;
}
static inline void Subtract(struct SceneVec *out, struct SceneVec *a, struct SceneVec *b)
{
    out->x = a->x - b->x;
    out->y = a->y - b->y;
    out->z = a->z - b->z;
}

static inline u16 DisplaySequence_GetIndex(struct DisplaySequenceState *state)
{
    return state->index;
}

static inline u32 DisplaySequence_ReadCommand(const u16 *command)
{
    return *command;
}

static inline u32 DisplaySequence_MaskCommand(u32 command, u32 mask)
{
    mask &= command;
    return mask;
}

/* Apply the current frame and flips, then advance the stored playback state.
 * The duration calculation uses the newly selected command's low four bits. */
static inline void SunDisc_StepAnimation(struct Disc *owner, struct DisplaySequenceState *state,
                                         struct Sprite *sprite)
{
    const u16 *command;
    {
        u32 offset = DisplaySequence_GetIndex(state) << 1;
        const u16 *table;
        table = state->commands;
        command = (const u16 *)((u32)table + offset);
    }
    owner->frame = *command >> 6;
    {
        union DisplaySequenceControl control;
        u32 bit, first;
        control.raw = state->flags;
        bit = 1;
        first = (u8)(control.raw & bit);
        if (first != (((*command & 0x30) >> 4) & bit))
            owner->flags |= 4;
        else
            owner->flags &= ~4;
    }
    {
        union DisplaySequenceControl control;
        u32 bit, first;
        control.raw = state->flags;
        bit = 2;
        first = (u8)(control.raw & bit);
        if (first != (((*command & 0x30) >> 4) & bit))
            sprite->flags |= 8;
        else
            sprite->flags &= ~8;
    }
    if (++state->elapsed >= state->duration) {
        state->elapsed = 0;
        if (state->flags & 4) {
            if (state->index == 0) {
                if (state->flags & 8)
                    state->elapsed = state->duration;
                else
                    state->index = state->frameCount - 1;
            } else {
                state->index--;
            }
        } else {
            if (++state->index >= state->frameCount) {
                if (state->flags & 8) {
                    state->elapsed = state->duration;
                    state->index = state->frameCount - 1;
                } else {
                    state->index = 0;
                }
            }
        }
        command = state->commands + state->index;
        state->frameWeight = DisplaySequence_MaskCommand(DisplaySequence_ReadCommand(command), 15);
        state->duration = (state->frameWeight * (s32)state->speed) >> 6;
        if (state->duration == 0)
            state->duration = 1;
    }
}

// confidence=low: updates a disc's sprite and position each frame, advancing its animation and falling state
void SunDiscPool_UpdateTravellingDisc(struct DiscPool *pool, struct Disc *p)
{
    struct Sprite *sprite = (struct Sprite *)p;
    struct DisplaySequenceState *anim = &p->anim;
    struct SceneVec ahead, delta;
    s16 *velocityZ;
    u16 height;
    if (p->entered) {
        p->flags &= ~1;
        p->age = 0;
        p->entered = 0;
    }
    SunDisc_StepAnimation(p, anim, sprite);
    Add(&sprite->position, &sprite->position, &p->velocity);
    p->projected = p->position;
    Polar(Math_Atan2(p->velocity.x, p->velocity.z), 32, &ahead);
    velocityZ = &p->velocity.z;
    Add(&ahead, &ahead, &sprite->position);
    if (!TestFalling(1, &p->falling)) {
        struct SceneVec *difference = &delta;
        struct SceneVec *previous = &p->projected;
        Subtract(difference, &p->target, previous);
        if (delta.x * delta.x + delta.z * delta.z <= 0xBD0F) {
            p->falling |= 1;
            p->velocity.y -= 4;
        }
    } else {
        p->velocity.y -= 4;
    }
    {
        u32 result = Height(&ahead) << 16;
        height = result >> 16;
    }
    Collider_SetPosition(p->collider, &sprite->position, 0);
    Collider_Refresh(p->collider);
    p->age++;
    if (sprite->position.y <= 0) {
        sprite->flags |= 1;
        pool->allocated &= ~(1 << p->slot);
        Sprite_Remove(p);
        p->state = 0;
    } else if (sprite->position.y <= (s16)height) {
        sprite->position.x = sprite->position.x - p->velocity.x;
        sprite->position.y = sprite->position.y - p->velocity.y;
        sprite->position.z = sprite->position.z - *velocityZ;
        p->velocity.x = 0;
        p->velocity.y = 0;
        *velocityZ = 0;
        p->state = 3;
        p->entered = 1;
        p->projected = p->position;
        Sound_PlaySong(242);
    }
}
