#include "global.h"
#include "engine.h"
#include "display_sequence_inline.h"
#include "scene_vec.h"
struct Sprite {
    u32 flags;
    u8 visible, unknown5, palette, unknown7;
    s8 scaleX, scaleY;
    u8 padA[6];
    u16 frame;
    u8 pad12[10];
    struct SceneVec position;
    u8 pad24[8];
    struct DisplaySequenceState anim;
    u8 pad3C[12];
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
u8 *TerrainModifiers_FindCellOverride(s32, s32);
void Sprite_Remove(void *);
void Sound_PlaySong(u32);
static inline void SunDisc_StepPlayback(struct Sprite *base, struct Sprite *sprite,
                                        struct DisplaySequenceState *state)
{
    const u16 *command;
    u32 flags;
    u32 bit;
    {
        u32 offset = state->index << 1;
        const u16 *table;
        table = base->anim.commands;
        command = (const u16 *)(((u32)table) + offset);
    }
    base->frame = (*command) >> 6;
    {
        union DisplaySequenceControl control;
        u32 first;
        u32 maskBit = 1;
        control.raw = state->flags;
        bit = maskBit;

        first = (u8)(control.raw & bit);
        if (first != ((((*command) & 0x30) >> 4) & bit))
            base->flags |= 4;
        else
            base->flags &= ~4;
    }
    {
        union DisplaySequenceControl control;
        u32 bit;
        u32 first;
        control.raw = state->flags;
        bit = 2;

        first = (u8)(control.raw & bit);

        if (first != ((((*command) & 0x30) >> 4) & bit))
            sprite->flags |= 8;
        else
            sprite->flags &= ~8;
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
            } else {
                state->index--;
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
            }
        }
        {
            u32 offset = state->index << 1;
            const u16 *table;
            table = state->commands;
            command = (const u16 *)((u32)table + offset);
        }
        state->frameWeight = DisplaySequence_MaskCommand(DisplaySequence_ReadCommand(command), 15);
        {
            s32 weight = state->frameWeight;
            s32 speed = state->speed;
            speed *= weight;
            speed >>= 6;
            state->duration = speed;
            if (!(u8)speed)
                state->duration = 1;
        }
    }
}

static inline u32 SunDisc_GetGroundHeight(struct SceneVec *position)
{
    s32 x = position->x >> 8;
    s32 z = position->z >> 8;
    s32 index;
    s32 height;
    s32 slope;
    u8 *cell;
    if ((((x < 0) || (z < 0)) || (((u32)x) >= gTerrainWidth)) || (((u32)z) >= gTerrainHeight))
        index = 0;
    else
        index = gTerrainMap->rows[z] + x;
    cell = TerrainModifiers_FindCellOverride(index, 1);
    if (cell)
        cell += 4;
    else
        cell = gTerrainMap->cells + ((index * 4) + 12);
    slope = (*cell) >> 4;
    height = ((*cell) & 15) << 8;
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

// confidence=low: disc falls under gravity, then enters its impact state at terrain height.
void SunDiscPool_UpdateFallingDisc(struct DiscPool *pool, struct Disc *p)
{
    struct Sprite *sprite = (struct Sprite *)p;
    u16 height;
    struct DisplaySequenceState *anim = &p->anim;

    if (p->entered) {
        p->flags &= ~1;
        p->age = 0;
        p->entered = 0;
    }

    SunDisc_StepPlayback((struct Sprite *)p, sprite, anim);
    p->velocity.y -= 4;
    {
        struct SceneVec *position = &sprite->position;
        struct SceneVec *velocity = &p->velocity;
        position->x = velocity->x + position->x;
        position->y = velocity->y + position->y;
        position->z = velocity->z + position->z;
    }
    p->projected = p->position;
    height = SunDisc_GetGroundHeight(&sprite->position);
    p->age++;
    if (sprite->position.y <= 0) {
        sprite->flags |= 1;
        pool->allocated &= ~(1 << p->slot);
        Sprite_Remove(p);
        p->state = 0;
    } else if (sprite->position.y <= ((s16)height)) {
        sprite->position.y = height;
        p->velocity.x = 0;
        p->velocity.y = 0;
        p->velocity.z = 0;
        p->state = 3;
        p->entered = 1;
        p->projected = p->position;
        Sound_PlaySong(242);
    }
}
