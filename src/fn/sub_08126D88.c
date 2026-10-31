#include "global.h"
#include "display_sequence.h"
#include "contact_position.h"
#include "ram.h"
#include "engine_globals.h"
struct Projectile {
    u32 flags;
    u8 unknown4[2], angle, unknown7[9];
    u16 frame;
    u8 unknown12[10];
    struct ContactPosition position;
    u8 unknown24[8];
    struct DisplaySequenceState sequence;
    u8 unknown3C[0x1C];
    u8 collider[0x58];
    struct ContactPosition velocity;
    u8 unknownB8;
    s8 spin;
    u8 spinScale, colliderFlags;
    u16 timer, state;
    u8 active;
    s8 slot;
    u8 unknownC2[2];
    u32 resourceId;
    u8 enter, soundDelay;
    u16 lifetime, cell;
};
struct ProjectilePool {
    u8 unknown0[0x860];
    u32 allocated;
};
struct TerrainMap {
    u32 unknown0;
    u8 *cells;
    u8 unknown8[0x1C];
    u16 rows[1];
};
void *TerrainModifiers_FindCellOverride(u32, u32);
s32 Math_Atan2(s32, s32);
s32 Div(s32, s32);
void Sound_PlaySong(u32);
void Sprite_Remove(void *);
void Collider_SetPosition(void *, void *, u32);
void Collider_Refresh(void *);
static inline s32 Cell(struct ContactPosition *p)
{
    s32 x = (s16)p->x >> 8, z = (s16)p->z >> 8;
    if (x < 0 || z < 0 || (u32)x >= gTerrainWidth || (u32)z >= gTerrainHeight)
        return 0;
    return ((struct TerrainMap *)gTerrainMap)->rows[z] + x;
}
static inline u8 *Properties(u32 cell)
{
    u8 *p = TerrainModifiers_FindCellOverride(cell, 1);
    if (p)
        return p + 4;
    return ((struct TerrainMap *)gTerrainMap)->cells + (cell * 4 + 12);
}
static inline s32 Height(u32 cell, struct ContactPosition *p)
{
    u8 *data = Properties(cell);
    u32 shape = *data >> 4;
    s32 height = (*data & 15) << 8;
    switch (shape) {
        case 1:
            height -= (u8)p->z;
            break;
        case 2:
            height -= (u8)p->x;
            break;
    }
    return height;
}
static inline u32 Attributes(u32 cell)
{
    return *(u16 *)(Properties(cell) + 2);
}
static inline void Sum(struct ContactPosition *out, struct ContactPosition *a,
                       struct ContactPosition *b)
{
    out->x = a->x + b->x;
    out->y = a->y + b->y;
    out->z = a->z + b->z;
}
static inline void Release(struct Projectile *p)
{
    struct ProjectilePool *pool = gPooledSpriteManager;
    s32 slot = p->slot;
    pool->allocated &= ~(1 << slot);
    p->active = 0;
    Sprite_Remove(p);
    p->slot = -1;
    p->state = 0;
    p->lifetime = 0;
}
// confidence=low: projectile identity inferred from the pool and collider callbacks.
void SpriteProjectile_UpdateRebounds(void *unused, struct Projectile *input)
{
    struct Projectile *p = input;
    struct Projectile *sprite = input;
    struct ContactPosition next;
    struct ContactPosition *position, *velocity;
    u32 cell, nextCell, attrs;
    s32 floor, nextFloor;
    u8 *delay;
    u16 *lifetime;
    if (p->enter) {
        p->flags &= ~1;
        p->enter = 0;
    }
    p->cell = Cell(&p->position);
    position = &sprite->position;
    velocity = &p->velocity;
    cell = p->cell;
    Math_Atan2((s16)velocity->x, (s16)velocity->z);
    floor = Height(cell, position);
    if ((s16)position->y > floor)
        velocity->y -= 4;
    Sum(&next, velocity, position);
    if ((s16)next.y < floor) {
        velocity->x = Div((s16)velocity->x * 5, 10);
        velocity->y = -Div((s16)velocity->y * 4, 10);
        velocity->z = Div((s16)velocity->z * 5, 10);
        if (p->flags & 2) {
            s8 *spin = &p->spin;
            s32 value = *spin;
            *spin = -value;
        }
        {
            u8 *source = &p->soundDelay;
            u32 value = *source;
            delay = source;
            if (!value) {
                u32 *resource;
                u32 *address = (void *)(source - 5);
                u32 firstResource = *address;
                {
                    u32 key = 0xB96C;
                    resource = address;
                    if (firstResource == key && p->sequence.unknownA == 6)
                        Sound_PlaySong(0x3C4);
                    else if (*resource == 0xD45E && p->sequence.unknownA == 6)
                        Sound_PlaySong(0x3B0);
                    {
                        u32 value = 10;
                        *delay = value;
                    }
                }
            }
        }
        {
            u16 value = floor;
            struct ContactPosition *dst;
            dst = position;
            dst->y = value;
        }
        if ((s16)velocity->x * (s16)velocity->x + (s16)velocity->y * (s16)velocity->y +
                (s16)velocity->z * (s16)velocity->z ==
            0) {
            velocity->x = 0;
            velocity->y = 0;
            velocity->z = 0;
            p->spin = 0;
            p->active = 3;
            p->enter = 1;
            goto stepped;
        }
    } else {
        {
            struct ContactPosition *dst = position;
            dst->y = next.y;
        }
        delay = &p->soundDelay;
    }
    nextCell = Cell(&next);
    nextFloor = Height(nextCell, &next);
    attrs = Attributes(nextCell);
    if (cell != nextCell && (floor < nextFloor || (Attributes(nextCell) & 3))) {
        velocity->x = -Div((s16)velocity->x * 5, 10);
        velocity->z = -Div((s16)velocity->z * 5, 10);
    }
    if ((nextFloor <= 0 || (attrs & 0x40)) && (s16)next.y <= nextFloor) {
        Release(p);
        lifetime = &p->lifetime;
    } else {
        position->x = position->x + velocity->x;
        position->z = position->z + velocity->z;
    stepped:
        lifetime = &p->lifetime;
    }
    {
        u32 flags;
        {
            struct Projectile *src = sprite;
            flags = src->flags;
        }
        if (flags & 2) {
            s32 amount = p->spinScale * p->spin;
            u32 angle;
            {
                struct Projectile *src = sprite;
                angle = src->angle;
            }
            amount += angle;
            {
                struct Projectile *dst = sprite;
                dst->angle = amount;
            }
        }
    }
    {
        struct DisplaySequenceState *state = &p->sequence;
        struct Projectile *base = p;
        const volatile u16 *command;
        u32 flags, bit;
        {
            u32 offset = state->index << 1;
            const volatile u16 *table;
            table = base->sequence.commands;
            command = (const volatile u16 *)((u32)table + offset);
        }
        {
            u32 frame = *command >> 6;
            struct Projectile *dst = sprite;
            dst->frame = frame;
        }
        {
            union DisplaySequenceControl input;
            u32 first;
            input.raw = state->flags;
            bit = 1;
            first = (u8)(input.raw & bit);
            if (first != (((*command & 0x30) >> 4) & bit)) {
                struct Projectile *dst = sprite;
                dst->flags |= 4;
            } else {
                struct Projectile *dst = sprite;
                dst->flags &= ~4;
            }
        }
        {
            union DisplaySequenceControl input;
            u32 bit;
            u32 first;
            input.raw = state->flags;
            bit = 2;
            first = (u8)(input.raw & bit);
            if (first != (((*command & 0x30) >> 4) & bit)) {
                u32 value;
                {
                    struct Projectile *src = sprite;
                    value = src->flags;
                }
                {
                    u32 mask = 8;
                    value |= mask;
                }
                {
                    struct Projectile *dst = sprite;
                    dst->flags = value;
                }
            } else {
                struct Projectile *dst = sprite;
                dst->flags &= ~8;
            }
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
                const volatile u16 *table;
                table = state->commands;
                command = (const volatile u16 *)((u32)table + offset);
            }
            state->frameWeight = *command & 15;
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
    if (p->colliderFlags & 1) {
        Collider_SetPosition(p->collider, &p->position, 0);
        Collider_Refresh(p->collider);
    }
    {
        u8 *dst = delay;
        u32 value;
        value = *dst;
        if (value)
            *dst = value - 1;
    }
    if (*lifetime)
        (*lifetime)--;
    else {
        p->active = 3;
        p->enter = 1;
    }
}
