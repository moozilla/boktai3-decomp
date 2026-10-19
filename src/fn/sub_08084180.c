#include "scene_actor_control_helpers.h"
void SceneActorControl_SetCollisionResponse(struct SceneActorRegistryResetView *input)
{
    s16 k2 = 0;
    u8 *field_2;
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
        struct SceneActorControlMotionState *state = SceneActorControl_GetMotionState(actor);
        u32 mask = 2;
        u32 offset = 0x3D4;
        u32 *flags;
        flags = (void *)((u8 *)state + offset);
        if (SceneActorControl_TestFlags(*flags, mask)) {
            u32 mask = 0x100;
            u16 *flags;
            u32 old;
            u32 zeroByte;
            u32 zeroWord;
            flags = &actor->flags2D4;
            old = *flags;
            zeroByte = k2;
            zeroWord = 0;
            mask |= old;
            *flags = mask;
            {
                u32 behavior = 3;
                u32 offset = 0x2A8;
                u8 *field;
                field_2 = ((u8 *)actor) + offset;
                *field_2 = zeroByte;
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
                u8 *field = &actor->field2B1;
                *field = 1;
            }
        }
    }
}
