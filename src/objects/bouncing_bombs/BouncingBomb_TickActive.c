#include "global.h"
#include "engine/engine.h"
#include "player/cutscene/player_cue_sequence.h"
#include "scene/motion/scene_actor_terrain.h"
#include "objects/bouncing_bombs/bouncing_object.h"

void Sound_PlaySong(u32 song);

static inline void UpdateCollidersAndAnimation(struct BouncingObject *obj,
                                               struct ContactSwarmSprite *sprite,
                                               struct BouncingPool *pool,
                                               struct ContactPosition *position)
{
    s16 delayIndex;
    u32 timer;
    u32 savedElapsed, savedIndex;
    Collider_SetPosition(&obj->removalCollider, position, 0);
    if (obj->collisionDelay != 0) {
        obj->collisionDelay--;
    } else {
        Collider_SetPosition(&obj->bodyCollider, position, 0);
        Collider_Refresh(&obj->bodyCollider);
    }
    if (obj->animationDelay) {
        obj->animationDelay--;
    } else {
        timer = obj->timer;
        if (timer <= 29)
            delayIndex = 3;
        else if (timer <= 49)
            delayIndex = 2;
        else if (timer <= 89)
            delayIndex = 1;
        else
            goto playback;
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
        obj->animationDelay = gBouncingObjectBlinkDelays[delayIndex];
    }
playback:
    PlayerCue_StepSequence(sprite, &obj->sequence);
}
static inline u32 TerrainBlockingFlags(s32 cell)
{
    u16 flags = ((u16 *)SceneActorTerrain_GetCell(cell))[1];
    return flags & 3;
}
static inline s32 SumVelocity(const struct ContactPosition *velocity)
{
    s32 x = velocity->x, z = velocity->z, y = velocity->y;
    return x + z + y;
}
void BouncingBomb_TickActive(struct BouncingPool *pool, struct BouncingObject *obj)
{
    struct ContactSwarmSprite *sprite = &obj->sprite;
    u16 *cellPtr;
    u16 cell;
    s32 ground;
    struct ContactPosition rel;
    struct ContactPosition next;
    struct ContactPosition *pos;
    struct ContactPosition *velocity;
    s32 nextCell, nextGround, speed;
    u32 cellFlags;

    if (obj->enter) {
        obj->sprite.flags &= ~1;
        obj->stateFrame = 0;
        obj->enter = 0;
    }
    obj->mapCell = SceneActorTerrain_GetCellIndex(&obj->sprite.position);
    if (obj->flags & 1) {
        if ((u32)sprite->position.y >
            (u32)SceneActorTerrain_GetHeight(obj->mapCell, &sprite->position))
            obj->velocity.y -= 4;
    } else {
        struct ContactPosition *delta = &rel;
        struct ContactPosition *here = &sprite->position;
        SceneActorTerrain_Subtract(delta, &obj->origin, here);
        if (delta->x * delta->x + delta->z * delta->z < 0x40000 || obj->stateFrame > 8) {
            obj->flags |= 1;
            obj->velocity.y -= 4;
        }
    }

    cellPtr = &obj->mapCell;
    pos = &sprite->position;
    velocity = &obj->velocity;
    cell = *cellPtr;
    Math_Atan2(velocity->x, velocity->z);
    ground = SceneActorTerrain_GetHeight(cell, pos);
    if (velocity->x + velocity->z + velocity->y != 0) {
        if (pos->y <= ground) {
            velocity->x = Div(6 * velocity->x, 10);
            velocity->y = -Div(4 * velocity->y, 10);
            velocity->z = Div(6 * velocity->z, 10);
            speed = SumVelocity(velocity);
            if (speed < 0)
                speed = -speed;
            if (speed <= 2) {
                velocity->x = 0;
                velocity->y = 0;
                velocity->z = 0;
            } else {
                Sound_PlaySong(0x166);
            }
            pos->y = ground;
        }
        next.x = pos->x + velocity->x;
        next.y = pos->y + velocity->y;
        next.z = pos->z + velocity->z;
        nextCell = SceneActorTerrain_GetCellIndex(&next);
        nextGround = SceneActorTerrain_GetHeight(nextCell, &next);
        cellFlags = ((u16 *)SceneActorTerrain_GetCell(nextCell))[1];
        if (cell != nextCell) {
            if (ground < nextGround || TerrainBlockingFlags(nextCell) != 0) {
                velocity->x = -Div(5 * velocity->x, 10);
                velocity->z = -Div(5 * velocity->z, 10);
                Sound_PlaySong(0x166);
            }
        }
        if ((nextGround <= 0 || (cellFlags & 0x40)) && next.y <= nextGround) {
            BouncingBomb_ReleaseSlot(obj);
        } else {
            pos->x += velocity->x;
            pos->y += velocity->y;
            pos->z += velocity->z;
        }
    }

    UpdateCollidersAndAnimation(obj, sprite, pool, &obj->sprite.position);
    obj->stateFrame++;
    if (obj->timer != 0) {
        obj->timer--;
    } else {
        obj->state = 2;
        obj->enter = 1;
    }
}
