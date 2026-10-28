// MATCHING_CONCESSION: register-pinning
#include "scene_actor_control_helpers.h"
#include "player.h"
void SceneActorControl_RestartPositionCurve(struct SceneActorRegistryResetView *actor)
{
    struct SceneActorControlMotionState *state = SceneActorControl_GetMotionState(actor);
    u32 fill;
    register u32 zero asm("r5");
    {
        register u32 offset asm("r2") = 0x3E2;
        register u8 *field asm("r0");
        asm("" : "+r"(offset));
        field = (u8 *)state + offset;
        zero = 0;
        *field = zero;
    }
    {
        u32 offset = 0x2EC;
        void *destination;
        destination = (u8 *)actor + offset;
        fill = zero;
        {
            const void *source = &fill;
            u32 control;
            asm("" : "+r"(source));
            control = 0x05000002;
            CpuSet(source, destination, control);
        }
    }
    {
        u32 offset = 0x3D4;
        u32 mask;
        state = (void *)((u8 *)state + offset);
        mask = ~7;
        *(u32 *)state &= mask;
    }
    {
        u8 *status = (u8 *)actor + 0xDC;
        u32 mask = 8;
        mask |= status[0xF];
        status[0xF] = mask;
    }
    {
        u32 mask = 0x100000;
        u32 offset = 0x2D0;
        u32 *flags;
        flags = (void *)((u8 *)actor + offset);
        *flags |= mask;
    }
    {
        u32 mask = 0x100;
        u32 offset = 0x2D4;
        u16 *flags;
        u32 old;
        u32 zeroByte;
        flags = (void *)((u8 *)actor + offset);
        old = *flags;
        zeroByte = 0;
        mask |= old;
        *flags = mask;
        {
            register u32 phase asm("r1") = 2;
            u32 offset = 0x2A8;
            register u8 *field asm("r0");
            field = (u8 *)actor + offset;
            *field = zeroByte;
            offset += 1;
            field = (u8 *)actor + offset;
            *field = phase;
            {
                u32 offset = 0x2AA;
                field = (u8 *)actor + offset;
                *field = zeroByte;
            }
            offset += 3;
            field = (u8 *)actor + offset;
            *(u32 *)field = zero;
        }
    }
    {
        register u8 *field asm("r1") = &actor->field2B1;
        asm("" : "+r"(field));
        *field = 1;
    }
    ActorShadow_Enable((u8 *)actor + 0x320);
    SceneActorControl_StartPositionCurve(actor, (void *)&gPlayers[0]->position);
}
