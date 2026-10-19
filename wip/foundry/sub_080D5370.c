#define SceneActor_AdvanceTimedBehavior sub_080D5370
#include "global.h"
struct SceneActorBehaviorParameters {
    u8 unknown0[0x44];
    u16 duration;
};
struct SceneActorTimedBehavior {
    u8 unknown0[0x98];
    u8 category;
    u8 unknown99[0x1D7];
    void *callback;
    u8 unknown274[0x34];
    u8 phase, step, state, unknown2AB;
    s32 timer;
    u8 substate, resetRequested, prepared;
    u8 unknown2B3[0x121];
    struct SceneActorBehaviorParameters parameters;
};
void sub_0807C72C(void *, u32, u32, u32);
s32 sub_0824923C(void *, void *);
void sub_080D5370(u8 *actor)
{
    struct SceneActorBehaviorParameters *parameters =
        (struct SceneActorBehaviorParameters *)((u32)actor + 0x3D4);
    u8 busy;
    if (*(u8 *)((u32)actor + 0x2B1))
        *(u8 *)((u32)actor + 0x2B1) = 0;
    busy = sub_0824923C(actor, *(void **)(actor + 0x270));
    if (!busy) {
        if (*(s32 *)((u32)actor + 0x2AC) >= parameters->duration) {
            sub_0807C72C(actor, 0, 2, actor[0x98]);
            {
                u32 state = 2;
                *(u8 *)((u8 *)actor + 0x2A8) = 0;
                *(u8 *)((u8 *)actor + 0x2A9) = 0;
                *(u8 *)((u8 *)actor + 0x2AA) = state;
                *(s32 *)((u8 *)actor + 0x2AC) = 0;
                *(u8 *)((u8 *)actor + 0x2B1) = 1;
            }
        } else {
            (*(s32 *)((u32)actor + 0x2AC))++;
        }
    }
}
