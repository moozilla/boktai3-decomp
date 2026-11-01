#include "global.h"
#include "engine.h"
#include "player_cue_sequence.h"
#include "scene_actor_terrain.h"
#include "bouncing_object.h"

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
    u32 mask = 3;
    return flags & mask;
}
static inline s32 SumVelocity(const struct ContactPosition *velocity)
{
    s32 x = velocity->x, z = velocity->z, y = velocity->y;
    return x + z + y;
}
void BouncingObject_TickActive(struct BouncingPool *pool, struct BouncingObject *input)
{
    struct BouncingObject *obj = input;
    struct ContactSwarmSprite *sprite = &input->sprite;
    u16 *cellPtr;
    u16 currentCell;
    s32 currentGround;
    struct ContactPosition rel;
    union {
        struct ContactPackedVector packed;
        struct ContactPosition position;
    } candidate;
    struct ContactPosition *pos;
    struct ContactPosition *velocity;
    s32 cell, ground, nextGround, speed, dist2;
    u32 cellFlags;

    if (obj->enter) {
        obj->sprite.flags &= ~1;
        obj->stateFrame = 0;
        obj->enter = 0;
    }
    obj->mapCell = SceneActorTerrain_GetCellIndex(&obj->sprite.position);
    if (obj->flags & 1) {
        s32 height = sprite->position.y;
        s32 oldCell = obj->mapCell;
        struct ContactPosition *here = &sprite->position;
        ground = SceneActorTerrain_GetHeight(oldCell, here);
        if ((u32)height > (u32)ground)
            obj->velocity.y -= 4;
    } else {
        struct ContactPosition *delta = &rel;
        struct ContactPosition *here = &sprite->position;
        struct ContactPosition *origin = &obj->origin;
        SceneActorTerrain_Subtract(delta, origin, here);
        dist2 = delta->x * delta->x + delta->z * delta->z;
        if (dist2 <= 0x3ffff || obj->stateFrame > 8) {
            obj->flags |= 1;
            obj->velocity.y -= 4;
        }
    }

    cellPtr = &obj->mapCell;
    pos = &sprite->position;
    velocity = &obj->velocity;
    currentCell = *cellPtr;
    Math_Atan2(velocity->x, velocity->z);
    currentGround = SceneActorTerrain_GetHeight(currentCell, pos);
    if (velocity->x + velocity->z + velocity->y != 0) {
        if (pos->y <= currentGround) {
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
            pos->y = currentGround;
        }
        candidate.packed.x = pos->x + velocity->x;
        candidate.packed.y = pos->y + velocity->y;
        candidate.packed.z = pos->z + velocity->z;
        cell = SceneActorTerrain_GetCellIndex(&candidate.position);
        nextGround = SceneActorTerrain_GetHeight(cell, &candidate.position);
        cellFlags = ((u16 *)SceneActorTerrain_GetCell(cell))[1];
        if (currentCell != cell) {
            if (currentGround < nextGround || TerrainBlockingFlags(cell) != 0) {
                velocity->x = -Div(5 * velocity->x, 10);
                velocity->z = -Div(5 * velocity->z, 10);
                Sound_PlaySong(0x166);
            }
        }
        if ((nextGround <= 0 || (cellFlags & 0x40)) && candidate.position.y <= nextGround) {
            BouncingObject_ReleaseSlot(obj);
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
