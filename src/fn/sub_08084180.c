// MATCHING_CONCESSION: register-pinning
#include "scene_actor_control_helpers.h"
void SceneActorControl_SetCollisionResponse(struct SceneActorRegistryResetView *input)
{
    struct SceneActorRegistryResetView *actor = input;
    u8 *status = (u8 *)actor + 0xDC;
    u32 statusMask = 8;
    u32 testMask;
    u16 *field;
    statusMask |= status[0xF];
    status[0xF] = statusMask;
    testMask = 0x100;
    field = &actor->flags2D4;
    if (!(*field & testMask)) {
        register struct SceneActorControlMotionState *state asm("r0") =
            SceneActorControl_GetMotionState(actor);
        register u32 mask asm("r1") = 2;
        u32 offset = 0x3D4;
        u32 *flags;
        asm("" : "+r"(mask));
        flags = (void *)((u8 *)state + offset);
        if (SceneActorControl_TestFlags(*flags, mask)) {
            u32 mask = 0x100;
            register u16 *flags asm("r2");
            u32 old;
            u32 zeroByte;
            u32 zeroWord;
            flags = &actor->flags2D4;
            old = *flags;
            zeroByte = 0;
            zeroWord = 0;
            asm("" : "+r"(zeroByte), "+r"(zeroWord));
            mask |= old;
            *flags = mask;
            {
                u32 behavior = 3;
                u32 offset = 0x2A8;
                register u8 *field asm("r0");
                field = (u8 *)actor + offset;
                *field = zeroByte;
                offset = 0x2A9;
                field = (u8 *)actor + offset;
                *field = zeroByte;
                offset = 0x2AA;
                field = (u8 *)actor + offset;
                *field = behavior;
                offset = 0x2AC;
                field = (u8 *)actor + offset;
                *(u32 *)field = zeroWord;
            }
            {
                register u8 *field asm("r1") = &actor->field2B1;
                *field = 1;
            }
        }
    }
}
