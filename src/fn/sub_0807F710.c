#include "scene_actor_reset.h"
#include "scene_actor_contact_list.h"
#include "scene_actor_behavior.h"
struct SceneActorTargetLink {
    u8 *target;
    u8 unknownE0[0xB];
    u8 flags;
};
void SceneActorTarget_ResetContactResponse(struct SceneActorBehaviorView *);
static inline u8 TargetHasFlag(struct SceneActorTargetLink *state)
{
    u32 mask = 2;
    if (state->flags & mask)
        return 1;
    return 0;
}
s32 SceneActorTarget_ResolvePendingObject(struct SceneActorRegistryResetView *actor)
{
    struct SceneActorRegistryResetView *owner = actor;
    s32 *pendingId = (void *)((u8 *)owner + 0x304);
    if (*pendingId > 0) {
        struct SceneActorTargetLink *state = (void *)((u8 *)owner + 0xDC);
        struct Delta {
            s16 x, y, z, pad;
        } delta;
        struct ContactPosition *target;
        s32 x, z;
        u32 cell;
        SceneActorTarget_ResetContactResponse((void *)owner);
        {
            u16 *flags = &owner->group.flags414;
            u32 mask;
            mask = ~2;
            mask &= *flags;
            *flags = mask;
        }
        {
            u32 mask = 0x80;
            u16 *flags;
            flags = &owner->flags2D6;
            mask |= *flags;
            mask &= ~0x100;
            *flags = mask;
        }
        {
            u32 mask = 0x10;
            state->flags = (*(&mask)) | state->flags;
        }
        {
            u8 *object = SceneActorContacts_FindObjectById(*pendingId);
            state->target = object;
            x = ((struct ContactPosition *)(object + 0xC))->x >> 8;
            z = ((struct ContactPosition *)(object + 0xC))->z >> 8;
        }
        if ((((x < 0) || (z < 0)) || (((u32)x) >= gTerrainWidth)) || (((u32)z) >= gTerrainHeight)) {
            cell = 0;
        } else {
            cell = gTerrainMap->rows[z] + x;
        }
        *(u16 *)((u8 *)owner + 0x2F8) = cell;
        {
            struct Delta *out = &delta;
            struct ContactPosition *source = &owner->body.position;
            target = (void *)(state->target + 0xC);
            {
                u32 xDelta = (u16)target->x;
                struct Delta *first;
                xDelta -= (u16)source->x;
                first = &delta;
                first->x = xDelta;
            }
            out->y = (u16)target->y - (u16)source->y;
            out->z = (u16)target->z - (u16)source->z;
            {
                struct Delta *first = &delta;
                u32 direction;
                u8 *field;
                direction = Math_Atan2(first->x, out->z);
                field = &owner->body.direction;
                *field = direction;
            }
        }
        return 1;
    } else {
        struct SceneActorTargetLink *state = (void *)((u8 *)owner + 0xDC);
        if (TargetHasFlag(state)) {
            u32 mask = ~0x10;
            mask &= state->flags;
            state->flags = mask;
        }
        return 0;
    }
}
