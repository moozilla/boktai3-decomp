#include "global.h"

#include "engine.h"
#include "contact_actor.h"
#include "bouncing_object.h"
#include "player_cue_sequence.h"
extern const u8 gUnk_08605B68[];

u8 *TerrainModifiers_FindCellOverride(s32, s32);
s32 Div(s32, s32);
void Sound_PlaySong(u32 song);

static inline s32 Cell(struct ContactPosition *p)
{
    s32 x = (s16)p->x >> 8, z = (s16)p->z >> 8;
    if (x < 0 || z < 0 || (u32)x >= gTerrainWidth || (u32)z >= gTerrainHeight)
        return 0;
    return gTerrainMap->rows[z] + x;
}
static inline u8 *Properties(u32 cell)
{
    u8 *p = TerrainModifiers_FindCellOverride(cell, 1);
    if (p)
        return p + 4;
    return gTerrainMap->cells + (cell * 4 + 12);
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

// confidence=low: per-frame update of one bouncing object (ground contact, velocity, sequence, timer)
void sub_08068BE4(struct BouncingPool *pool, struct BouncingObject *input)
{
    struct BouncingObject *obj = input;
    struct BouncingObject *sprite = input;
    struct ContactPosition rel;
    struct ContactPosition next;
    struct ContactPosition *position, *velocity;
    u32 cell, nextCell, attrs;
    s32 floor, nextFloor;
    u8 *delay;
    u16 *flags;
    u32 value, test;

    if (obj->enter) {
        obj->sprite.flags &= ~1;
        obj->stateFrame = 0;
        obj->enter = 0;
    }
    {
        u16 c = Cell(&obj->sprite.position);
        obj->mapCell = c;
    }
    flags = &obj->flags;
    value = *flags;
    test = 1;
    test &= value;
    if (test) {
        s32 y = sprite->sprite.position.y;
        u32 c = obj->mapCell;
        struct ContactPosition *at = &sprite->sprite.position;
        floor = Height(c, at);
        if ((u32)y > (u32)floor)
            obj->velocity.y -= 4;
    } else {
        s32 dist2;
        struct ContactPosition *d = &rel;
        struct ContactPosition *at = &sprite->sprite.position;
        struct ContactPosition *o = &obj->origin;
        d->x = o->x - sprite->sprite.position.x;
        d->y = o->y - at->y;
        d->z = o->z - at->z;
        dist2 = d->x * d->x + d->z * d->z;
        if (dist2 <= 0x3FFFF || obj->stateFrame > 8) {
            u32 mask = 1;
            mask |= value;
            *flags = mask;
            obj->velocity.y -= 4;
        }
    }
    {
        u16 *mc = &obj->mapCell;
        position = &sprite->sprite.position;
        velocity = &obj->velocity;
        cell = *mc;
    }
    Math_Atan2(velocity->x, velocity->z);
    floor = Height(cell, position);
    if (velocity->x + velocity->z + velocity->y != 0) {
        if (position->y <= floor) {
            velocity->x = Div(velocity->x * 6, 10);
            velocity->y = -Div(velocity->y * 4, 10);
            velocity->z = Div(velocity->z * 6, 10);
            {
                s32 x = velocity->x, z = velocity->z, y = velocity->y;
                s32 speed = x + z + y;
                if (speed < 0)
                    speed = -speed;
                if (speed <= 2) {
                    velocity->x = 0;
                    velocity->y = 0;
                    velocity->z = 0;
                } else {
                    Sound_PlaySong(0x166);
                }
            }
            position->y = floor;
        }
        next.x = position->x + velocity->x;
        next.y = position->y + velocity->y;
        next.z = position->z + velocity->z;
        nextCell = Cell(&next);
        nextFloor = Height(nextCell, &next);
        attrs = Attributes(nextCell);
        if (cell != nextCell && (floor < nextFloor || (Attributes(nextCell) & 3))) {
            velocity->x = -Div(velocity->x * 5, 10);
            velocity->z = -Div(velocity->z * 5, 10);
            Sound_PlaySong(0x166);
        }
        if ((nextFloor <= 0 || (attrs & 0x40)) && next.y <= nextFloor) {
            struct BouncingPool *bp = gBouncingPool;
            s8 *index = &obj->index;
            s32 slot = *index;
            bp->used &= ~(1 << slot);
            obj->state = 0;
            Sprite_Remove(obj);
            Shadow_Destroy(obj->registration);
            Collider_Remove(&obj->removalCollider);
            *(u8 *)index = 255;
            obj->flags = 0;
            obj->timer = 0;
        } else {
            position->x += velocity->x;
            position->y += velocity->y;
            position->z += velocity->z;
        }
    }
    Collider_SetPosition(&obj->removalCollider, &obj->sprite.position, 0);
    if (obj->collisionDelay != 0) {
        obj->collisionDelay--;
    } else {
        Collider_SetPosition(&obj->bodyCollider, &obj->sprite.position, 0);
        Collider_Refresh(&obj->bodyCollider);
    }
    {
        u32 d = obj->animationDelay;
        delay = &obj->animationDelay;
        if (d != 0) {
            *delay = d - 1;
            goto step;
        }
    }
    {
        u32 idx;
        u16 savedElapsed, savedIndex;
        u32 timer = obj->timer;
        if (timer <= 0x1D)
            idx = 3;
        else if (timer <= 0x31)
            idx = 2;
        else if (timer <= 0x59)
            idx = 1;
        else
            goto step;
        savedElapsed = obj->sequence.elapsed;
        savedIndex = obj->sequence.index;
        if (obj->animation == 1) {
            Anim_SetSequence(&obj->sequence, pool->sequenceResource, 2, 0, 0);
            obj->animation = 2;
        } else {
            Anim_SetSequence(&obj->sequence, pool->sequenceResource, 1, 0, 0);
            obj->animation = 1;
        }
        obj->sequence.elapsed = savedElapsed;
        obj->sequence.index = savedIndex;
        *delay = gUnk_08605B68[idx];
    }
step:
    PlayerCue_StepSequence(&sprite->sprite, &obj->sequence);
    obj->stateFrame++;
    if (obj->timer != 0) {
        obj->timer--;
    } else {
        obj->state = 2;
        obj->enter = 1;
    }
}
