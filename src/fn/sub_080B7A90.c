// MATCHING_CONCESSION: register-pinning
/* Skeleton variant of SceneActorTarget_ResolvePendingObject that also clears state bits 0x300 */
#include "global.h"
// confidence=low: resolves the pending contact object and clears Skeleton context state bits.
#include "scene_actor_reset.h"
#include "scene_actor_contact_list.h"
#include "scene_actor_behavior.h"
struct SceneActorTargetLink {
    u8 *target;
    u8 unknownE0[0xB];
    u8 flags;
};
void SceneActorTarget_ResetContactResponse(struct SceneActorBehaviorView *);
static inline u8 TargetHasFlag(struct SceneActorTargetLink *state,
                               struct SceneActorTargetLink **saved)
{
    register struct SceneActorTargetLink *input asm("r2") = state;
    u32 mask = 2;
    u32 value;
    value = input->flags & mask;
    {
        register struct SceneActorTargetLink *copy asm("r4") = input;
        asm("" : "+r"(copy), "+r"(value));
        *saved = copy;
    }
    if (value)
        return 1;
    return 0;
}
s32 SceneActorTarget_ResolvePendingObjectSkeleton(struct SceneActorRegistryResetView *actor)
{
    struct SceneActorRegistryResetView *owner = actor;
    s32 *pendingId = (void *)((u8 *)owner + 0x304);
    if (*pendingId > 0) {
        struct SceneActorTargetLink *state;
        struct Delta {
            s16 x, y, z, pad;
        } delta;
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
            flags = &owner->flags2D6;
            mask |= *flags;
            *flags = mask;
        }
        state = (void *)((u8 *)owner + 0xDC);
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
        {
            u32 *sf = (u32 *)(*(u8 **)((u8 *)owner + 0x3D0) + 0x6DC);
            u32 mask = 0xFFFFFCFF;
            *sf &= mask;
        }
        return 1;
    } else {
        struct SceneActorTargetLink *state = (void *)((u8 *)owner + 0xDC);
        if (TargetHasFlag(state, &state)) {
            u32 mask = ~0x10;
            mask &= state->flags;
            state->flags = mask;
        }
        return 0;
    }
}
