#include "global.h"
#ifdef NONMATCHING
// fuzzy: 85.7%
// foundry draft (worker best 85.7%); what differs is mostly register allocation and instruction order
#define ActorReaction_UpdateCallbackGate sub_081A1E5C
#define gActorCallbackGateFlags gUnk_02000560
extern u32 gUnk_02000560;
struct ActorReaction {
    u8 unknown0[0x9C];
    u32 flags;
    u8 unknownA0, flagsA1;
    u8 unknownA2[0xD];
    u8 callbackBlocked;
};
void SceneActors_RunStateCallback(u32);
void sub_081A1E5C(u32 raw)
{
    struct ActorReaction *actor = (struct ActorReaction *)raw;
    actor->callbackBlocked = 0;
    { u32 mask = 4;
    if (!(gActorCallbackGateFlags & mask)) {
        u32 one = 1;
        if (actor->flagsA1 & one) {
            actor->flags = 0;
            actor->callbackBlocked = one;
        } else {
            SceneActors_RunStateCallback(raw);
        }
    }
    }
}
#else
INCLUDE_ASM("asm/nonmatching", sub_081A1E5C);
#endif
