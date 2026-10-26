#include "global.h"

#include "engine.h"
#include "contact_actor.h"
#include "bouncing_object.h"
extern const u8 gUnk_08605B68[];

u8 *TerrainModifiers_FindCellOverride(s32, s32);
s32 Div(s32, s32);
void Sound_PlaySong(u32 song);

static inline u8 *sub08068BE4_CellDefinition(s32 cell)
{
    u8 *definition = TerrainModifiers_FindCellOverride(cell, 1);
    if (definition)
        return definition + 4;
    return gTerrainMap->cells + cell * 4 + 12;
}

static inline s32 sub08068BE4_Height(u8 *definition, struct ContactPosition *position)
{
    u32 kind = *definition >> 4;
    s32 height = (*definition & 15) << 8;
    switch (kind) {
        case 1:
            height -= (u8)position->z;
            break;
        case 2:
            height -= (u8)position->x;
            break;
    }
    return height;
}

static inline s32 sub08068BE4_CellIndex(struct ContactPosition *position)
{
    s32 x = position->x >> 8;
    s32 z = position->z >> 8;
    if (x < 0 || z < 0 || (u32)x >= gTerrainWidth || (u32)z >= gTerrainHeight)
        return 0;
    return gTerrainMap->rows[z] + x;
}

#ifdef NONMATCHING
// fuzzy: 45.1%
// scope shape 321 (differences once low registers are set aside)
// confidence=low: per-frame update of one bouncing object (ground contact, velocity, sequence, timer)
void sub_08068BE4(struct BouncingPool *pool, struct BouncingObject *obj)
{
    struct ContactPosition rel;
    struct ContactPosition np;
    struct ContactPosition *pos;
    struct ContactPosition *position = &obj->sprite.position;
    struct ContactPosition *velocity = &obj->velocity;
    struct DisplaySequenceState *seq = &obj->sequence;
    s32 cell, ground, ground2, ground3, speed, dist2, idx, timer;
    u32 cellFlags;
    u8 *definition;
    u16 savedElapsed, savedIndex;
    u8 animation;

    if (obj->enter) {
        obj->sprite.flags &= ~2;
        obj->stateFrame = 0;
        obj->enter = 0;
    }

    cell = sub08068BE4_CellIndex(position);
    obj->mapCell = cell;

    if (obj->flags & 1) {
        ground = sub08068BE4_Height(sub08068BE4_CellDefinition(obj->mapCell), position);
        pos = position;
        if ((u32)position->y <= (u32)ground)
            goto afterGravity;
    } else {
        rel.x = obj->origin.x - position->x;
        rel.y = obj->origin.y - position->y;
        rel.z = obj->origin.z - position->z;
        dist2 = rel.x * rel.x + rel.z * rel.z;
        pos = &rel;
        if (!(dist2 <= 0x3FFFF || obj->stateFrame > 8))
            goto afterGravity;
        obj->flags |= 1;
    }
    velocity->y -= 4;
afterGravity:

    cell = obj->mapCell;
    Math_Atan2(velocity->x, velocity->z);
    ground2 = sub08068BE4_Height(sub08068BE4_CellDefinition(cell), pos);

    if (velocity->x + velocity->z + velocity->y != 0) {
        if (pos->y <= ground2) {
            velocity->x = Div(6 * velocity->x, 10);
            velocity->y = -Div(4 * velocity->y, 10);
            velocity->z = Div(6 * velocity->z, 10);
            speed = velocity->x + velocity->z + velocity->y;
            if (speed < 0)
                speed = -speed;
            if (speed > 2) {
                Sound_PlaySong(0x166);
            } else {
                velocity->x = 0;
                velocity->y = 0;
                velocity->z = 0;
            }
            pos->y = ground2;
        }

        np.x = velocity->x + pos->x;
        np.y = velocity->y + pos->y;
        np.z = velocity->z + pos->z;
        cell = sub08068BE4_CellIndex(&np);
        ground3 = sub08068BE4_Height(sub08068BE4_CellDefinition(cell), &np);
        cellFlags = ((u16 *)sub08068BE4_CellDefinition(cell))[1];

        if (cell != obj->mapCell) {
            if (ground2 < ground3 || (((u16 *)sub08068BE4_CellDefinition(cell))[1] & 3) != 0) {
                velocity->x = -Div(5 * velocity->x, 10);
                velocity->z = -Div(5 * velocity->z, 10);
                Sound_PlaySong(0x166);
            }
        }

        if (!(ground3 > 0 && (cellFlags & 0x40) == 0) && np.y <= ground3) {
            gBouncingPool->used &= ~(1u << obj->index);
            obj->state = 0;
            Sprite_Remove(obj);
            Shadow_Destroy((u8 *)obj + 0xE4);
            Collider_Remove(&obj->removalCollider);
            obj->index = -1;
            obj->flags = 0;
            obj->timer = 0;
            goto collider;
        }

        pos->x += velocity->x;
        pos->y += velocity->y;
        pos->z += velocity->z;
    }

collider:
    Collider_SetPosition(&obj->removalCollider, position, 0);
    if (obj->collisionDelay != 0) {
        obj->collisionDelay--;
    } else {
        Collider_SetPosition(&obj->bodyCollider, position, 0);
        Collider_Refresh(&obj->bodyCollider);
    }

    if (obj->animationDelay != 0) {
        obj->animationDelay--;
    } else if (obj->timer <= 0x59) {
        timer = obj->timer;
        if (timer <= 0x1D)
            idx = 3;
        else if (timer <= 0x31)
            idx = 2;
        else
            idx = 1;
        savedElapsed = seq->elapsed;
        savedIndex = seq->index;
        if (obj->animation == 1) {
            Anim_SetSequence(seq, pool->sequenceResource, 2, 0, 0);
            animation = 2;
        } else {
            Anim_SetSequence(seq, pool->sequenceResource, 1, 0, 0);
            animation = 1;
        }
        obj->animation = animation;
        seq->elapsed = savedElapsed;
        seq->index = savedIndex;
        obj->animationDelay = gUnk_08605B68[idx];
    }

    obj->sprite.frame = ((u16 *)seq->commands)[seq->index] >> 6;
    if ((seq->flags & 1) != ((((u16 *)seq->commands)[seq->index] >> 4) & 1))
        obj->sprite.flags |= 4;
    else
        obj->sprite.flags &= ~5;
    if ((seq->flags & 2) != ((((u16 *)seq->commands)[seq->index] >> 4) & 2))
        obj->sprite.flags |= 8;
    else
        obj->sprite.flags &= ~9;

    seq->elapsed++;
    if (seq->elapsed >= seq->duration) {
        seq->elapsed = 0;
        if (seq->flags & 4) {
            if (seq->index != 0) {
                seq->index = seq->index - 1;
            } else if (seq->flags & 8) {
                seq->elapsed = seq->duration;
            } else {
                seq->index = seq->frameCount - 1;
            }
        } else {
            seq->index = seq->index + 1;
            if (seq->index >= seq->frameCount) {
                if (seq->flags & 8) {
                    seq->elapsed = seq->duration;
                    seq->index = seq->frameCount - 1;
                } else {
                    seq->index = 0;
                }
            }
        }
        seq->frameWeight = ((u16 *)seq->commands)[seq->index] & 0xF;
        seq->duration = (seq->speed * seq->frameWeight) >> 6;
        if ((u8)seq->duration == 0)
            seq->duration = 1;
    }

    obj->stateFrame++;
    if (obj->timer != 0) {
        obj->timer--;
    } else {
        obj->state = 2;
        obj->enter = 1;
    }
}
#else
INCLUDE_ASM("asm/nonmatching", sub_08068BE4);
#endif
