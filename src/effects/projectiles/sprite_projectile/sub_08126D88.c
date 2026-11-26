#include "engine/video/display_sequence_sprite.h"
#include "effects/projectiles/sprite_projectile.h"
#include "scene/terrain/terrain_sampling.h"
#include "global.h"
#include "engine/video/display_sequence_inline.h"
#include "scene/actor/contact_position.h"
#include "engine/ram.h"
#include "engine/engine_globals.h"
s32 Math_Atan2(s32, s32);
s32 Div(s32, s32);
static inline u8 *Properties(u32 cell)
{
    u8 *p = TerrainModifiers_FindCellOverride(cell, 1);
    if (p)
        return p + 4;
    return (u8 *)&((struct TerrainGrid *)gTerrainMap->cells)->cells[cell];
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
        p->sprite.flags &= ~1;
        p->enter = 0;
    }
    p->cell = Terrain_GetCellIndex(&p->sprite.position);
    position = &sprite->sprite.position;
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
        if (p->sprite.flags & 2) {
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
                u32 *address = &p->resourceId;
                u32 firstResource = *address;
                {
                    u32 key = 0xB96C;
                    resource = address;
                    if (firstResource == key && p->sequence.sequenceId == 6)
                        Sound_PlaySong(0x3C4);
                    else if (*resource == 0xD45E && p->sequence.sequenceId == 6)
                        Sound_PlaySong(0x3B0);
                    {
                        *delay = 10;
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
    nextCell = Terrain_GetCellIndex(&next);
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
            flags = src->sprite.flags;
        }
        if (flags & 2) {
            s32 amount = p->spinScale * p->spin;
            u32 angle;
            {
                struct Projectile *src = sprite;
                angle = src->sprite.attribute;
            }
            amount += angle;
            {
                struct Projectile *dst = sprite;
                dst->sprite.attribute = amount;
            }
        }
    }
    DisplaySequence_StepSprite(&sprite->sprite, &p->sequence);
    if (p->colliderFlags & 1) {
        Collider_SetPosition(p->collider, &p->sprite.position, 0);
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
