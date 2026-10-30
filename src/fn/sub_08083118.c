// MATCHING_CONCESSION: register-pinning
#include "scene_actor_control_phase.h"
void SceneActorControl_BeginSavedHeadingPhase(struct SceneActorRegistryResetView *actor)
{
    if (SceneActorControl_TakeFlag(&actor->field2B1))
        actor->body.direction = *((u8 *)actor + 0xE9);
    {
        u32 phase = 2;
        u32 offset = 0x2A8;
        u8 *state = (u8 *)actor + offset;
        register u32 phaseOffset asm("r4");
        u32 zero;
        zero = 0;
        *state = zero;
        phaseOffset = 0x2A9;
        *((u8 *)actor + phaseOffset) = phase;
        phase = 0x2AA;
        *((u8 *)actor + phase) = zero;
        phaseOffset += 3;
        {
            u32 *counter = (void *)((u8 *)actor + phaseOffset);
            *counter = zero;
        }
        actor->field2B1 = 1;
        {
            u32 *counter = (void *)((u8 *)actor + phaseOffset);
            asm("" : "+r"(counter) : "r"(phaseOffset));
            ++*counter;
        }
    }
}
