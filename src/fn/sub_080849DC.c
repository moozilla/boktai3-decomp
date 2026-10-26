// MATCHING_CONCESSION: register-pinning
#include "scene_actor_control_helpers.h"
void SceneActorControl_StartStoredCurve(struct SceneActorRegistryResetView *actor)
{
    struct ContactPosition position;
    gRandomIndex = (gRandomIndex + 1) & 0x3FF;
    position = *(struct ContactPosition *)((u8 *)actor + 0x2FC);
    SceneActorControl_StartPositionCurve(actor, &position);
    {
        u32 state = 2;
        u32 behavior = 3;
        register u8 *field asm("r0");
        u32 zero;
        {
            u32 firstOffset = 0x2A8;
            field = (u8 *)actor + firstOffset;
        }
        zero = 0;
        *field = state;
        {
            u32 offset = 0x2A9;
            field = (u8 *)actor + offset;
            *field = zero;
            offset += 1;
            field = (u8 *)actor + offset;
            *field = behavior;
            offset += 2;
            field = (u8 *)actor + offset;
            *(u32 *)field = zero;
        }
        {
            register u32 entryOffset asm("r0") = 0x2B1;
            u8 *entry;
            entry = (u8 *)actor + entryOffset;
            *entry = 1;
        }
        {
            u32 *flags = &SceneActorControl_GetMotionState(actor)->flags3D4;
            behavior -= 12;
            *flags &= behavior;
        }
        {
            u32 offset = 0x2B1;
            u8 *entry = (u8 *)actor + offset;
            *entry = zero;
        }
    }
}
