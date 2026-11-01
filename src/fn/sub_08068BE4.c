#include "global.h"
#include "engine.h"
#include "player_cue_sequence.h"
#include "scene_actor_terrain.h"
#include "bouncing_object.h"

void Sound_PlaySong(u32 song);

#ifdef NONMATCHING
// fuzzy: 74.3%
void BouncingObject_TickActive(struct BouncingPool *pool, struct BouncingObject *input)
{
    struct BouncingObject *obj = input;
    u16 currentCell;
    s32 currentGround;
    u8 *delay;
    struct ContactPosition rel;
    union {
        struct ContactPackedVector packed;
        struct ContactPosition position;
    } candidate;
    struct ContactPosition *pos;
    struct ContactPosition *velocity = &obj->velocity;
    struct DisplaySequenceState *seq;
    s32 cell, ground, nextGround, speed, dist2, delayIndex, timer;
    u32 cellFlags;
    u16 savedElapsed, savedIndex;
    u8 *animationSlot;
    struct ContactPosition *position;

    if (obj->enter) {
        obj->sprite.flags &= ~1;
        obj->stateFrame = 0;
        obj->enter = 0;
    }
    position = &obj->sprite.position;
    cell = SceneActorTerrain_GetCellIndex(position);
    obj->mapCell = cell;

    if (obj->flags & 1) {
        s32 y = obj->sprite.position.y;
        ground = SceneActorTerrain_GetHeight(obj->mapCell, &obj->sprite.position);
        pos = &obj->sprite.position;
        if ((u32)y > (u32)ground)
            obj->velocity.y -= 4;
    } else {
        SceneActorTerrain_Subtract(&rel, &obj->origin, &input->sprite.position);
        dist2 = rel.x * rel.x + rel.z * rel.z;
        pos = &input->sprite.position;
        if (dist2 <= 0x3ffff || obj->stateFrame > 8) {
            obj->flags |= 1;
            obj->velocity.y -= 4;
        }
    }
    currentCell = obj->mapCell;
    Math_Atan2(velocity->x, velocity->z);
    currentGround = SceneActorTerrain_GetHeight(currentCell, &input->sprite.position);
    if (velocity->x + velocity->z + velocity->y != 0) {
        if (pos->y <= currentGround) {
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
            pos->y = currentGround;
        }
        candidate.packed.x = velocity->x + pos->x;
        candidate.packed.y = velocity->y + pos->y;
        candidate.packed.z = velocity->z + pos->z;
        cell = SceneActorTerrain_GetCellIndex(&candidate.position);
        nextGround = SceneActorTerrain_GetHeight(cell, &candidate.position);
        cellFlags = ((u16 *)SceneActorTerrain_GetCell(cell))[1];
        if (cell != currentCell) {
            if (currentGround < nextGround ||
                (((u16 *)SceneActorTerrain_GetCell(cell))[1] & 3) != 0) {
                velocity->x = -Div(5 * velocity->x, 10);
                velocity->z = -Div(5 * velocity->z, 10);
                Sound_PlaySong(0x166);
            }
        }
        if ((nextGround <= 0 || (cellFlags & 0x40)) && candidate.position.y <= nextGround) {
            BouncingObject_ReleaseSlot(obj);
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
    delay = &obj->animationDelay;
    if (*delay) {
        (*delay)--;
        seq = &obj->sequence;
    } else {
        timer = obj->timer;
        if (timer <= 29)
            delayIndex = 3;
        else if (timer <= 49)
            delayIndex = 2;
        else {
            seq = &obj->sequence;
            if (timer > 89)
                goto playback;
            delayIndex = 1;
        }
        savedElapsed = obj->sequence.elapsed;
        savedIndex = obj->sequence.index;
        animationSlot = &obj->animation;
        if (*animationSlot == 1) {
            seq = &obj->sequence;
            Anim_SetSequence(seq, pool->sequenceResource, 2, 0, 0);
            *animationSlot = 2;
        } else {
            seq = &obj->sequence;
            Anim_SetSequence(seq, pool->sequenceResource, 1, 0, 0);
            *animationSlot = 1;
        }
        obj->sequence.elapsed = savedElapsed;
        obj->sequence.index = savedIndex;
        *delay = gBouncingObjectBlinkDelays[delayIndex];
    }
playback:
    PlayerCue_StepSequence(&input->sprite, seq);
    obj->stateFrame++;
    if (obj->timer != 0) {
        obj->timer--;
    } else {
        obj->state = 2;
        obj->enter = 1;
    }
}
#else
INCLUDE_ASM("asm/nonmatching", BouncingObject_TickActive);
#endif
