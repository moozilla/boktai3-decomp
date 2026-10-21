#include "global.h"
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
void ActorReaction_UpdateCallbackGate(struct ActorReaction *actor)
{
    struct ActorReaction *saved = actor;
    actor->callbackBlocked = 0;
    {
        u32 mask = 4;
        if (!(gActorCallbackGateFlags & mask)) {
            u32 one = 1;
            if (actor->flagsA1 & one) {
                actor->flags = 0;
                actor->callbackBlocked = one;
            } else {
                SceneActors_RunStateCallback((u32)saved);
            }
        }
    }
}
