#include "global.h"
#include "scene_actor_control_phase.h"
#include "scene_actor_animation.h"
#define SceneActorControl_ResumeOrFaceAway sub_08088C24
void sub_0808AF38(struct SceneActorRegistryResetView *);
static inline u8 Test(u32 value, u32 mask)
{
    if (value & mask)
        return 1;
    return 0;
}
void SceneActorControl_ResumeOrFaceAway(struct SceneActorRegistryResetView *input)
{
    struct SceneActorRegistryResetView *actor = input;
    struct SceneActorControlState *control = SceneActorControl_GetState(actor);
    if (SceneActorControl_TakeFlag(&actor->field2B2))
        return;
    {
        u32 mask = 2;
        u16 *flags = (void *)((u8 *)control + 0x628);
        u8 test = Test(*flags, mask);
        if (test) {
            if (*(u32 *)((u8 *)actor + 0xF0) > 0xFFFF)
                SceneActorAnimation_SelectDirectionalEntry((void *)actor, 0);
            sub_0808AF38(actor);
            {
                u32 clearMask = -3;
                u16 loaded = *flags;
                u8 zero;
                u32 zero2;
                clearMask &= loaded;
                zero2 = 0;
                zero = 0;
                *flags = clearMask;
                {
                    u8 behaviorVal = 0xF;
                    u32 offset = 0x2A8;
                    u8 *field = (u8 *)actor + offset;
                    *field = zero;
                    offset += 1;
                    field = (u8 *)actor + offset;
                    *field = zero;
                    actor->behavior2AA = behaviorVal;
                    offset += 3;
                    *(u32 *)((u8 *)actor + offset) = zero2;
                    actor->field2B1 = 1;
                }
            }
        } else {
            SceneActorAnimation_SelectDirectionalEntry((void *)actor, 1);
        }
    }
}
