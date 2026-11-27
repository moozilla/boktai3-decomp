#include "scene/collision/scene_actor_contact_response.h"
#include "scene/actor/scene_actor_reset.h"
#include "scene/collision/scene_actor_contact_list.h"
#include "scene/actor/scene_actor_behavior.h"
void SceneActorTarget_ResetContactResponse(struct RoomActor *);
s32 SceneActorTarget_ResolvePendingObject(struct ChaseActor *actor)
{
    struct ChaseActor *owner = actor;
    s32 *pendingId = &owner->base.counter304;
    if (*pendingId > 0) {
        struct SceneActorChaseTracking *state = &owner->base.tracking;
        struct ContactPosition delta;
        struct ContactPosition *target;
        s32 x, z;
        u32 cell;
        SceneActorTarget_ResetContactResponse((void *)owner);
        {
            u16 *flags = &owner->group.common.gate.flags;
            u32 mask;
            mask = ~2;
            mask &= *flags;
            *flags = mask;
        }
        {
            u32 mask = 0x80;
            u16 *flags;
            flags = &owner->base.reactionFlags;
            mask |= *flags;
            mask &= ~0x100;
            *flags = mask;
        }
        {
            u32 flags = 0x10;
            flags |= state->flags;
            state->flags = flags;
        }
        {
            struct SceneActorChaseTarget *object = SceneActorContacts_FindObjectById(*pendingId);
            state->target = object;
            x = object->position.x >> 8;
            z = object->position.z >> 8;
        }
        if ((((x < 0) || (z < 0)) || (((u32)x) >= gTerrainWidth)) || (((u32)z) >= gTerrainHeight)) {
            cell = 0;
        } else {
            cell = gTerrainMap->rows[z] + x;
        }
        SceneActorContact_SaveTargetCell(owner, cell);
        {
            struct ContactPosition *out = &delta;
            struct ContactPosition *source = &owner->base.body.position;
            target = &state->target->position;
            out->x = target->x - source->x;
            out->y = target->y - source->y;
            out->z = target->z - source->z;
            {
                struct ContactPosition *first = &delta;
                u32 direction;
                u8 *field;
                direction = Math_Atan2(first->x, out->z);
                field = &owner->base.body.direction;
                *field = direction;
            }
        }
        return 1;
    } else {
        struct SceneActorChaseTracking *state = &owner->base.tracking;
        if (SceneActorChase_TrackingFlags(state, 2)) {
            u32 mask = ~0x10;
            mask &= state->flags;
            state->flags = mask;
        }
        return 0;
    }
}
