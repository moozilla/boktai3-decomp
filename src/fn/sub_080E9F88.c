#include "global.h"
#include "scene_actor_reaction.h"

void Ivy_UpdateActiveState_080E98B4(void *);
void SceneActorReaction_PlayEntry17(struct ChaseActor *actor)
{
    if (SceneActorReaction_TakePrepared(actor)) {
        actor->base.colliderA.flags = (actor->base.colliderA.flags & 0xFFFB) | 8;
        {
            u16 *field = &actor->base.placementFlags;
            u32 mask = ~0x800;
            *field = mask & *field;
        }
        SceneActorAnimation_SelectDirectionalEntry((void *)actor, 17);
    }
    {
        u32 controlOffset = 0x2B3;
        if (*((u8 *)actor + controlOffset)) {
            void *callback = Ivy_UpdateActiveState_080E98B4;
            u8 kind = 34;
            u8 *prepared = &actor->base.field2B2;
            u32 zero = 0;
            u8 one = 1;
            u32 offset;
            u32 mask;
            u8 byteZero;
            *prepared = one;
            controlOffset -= 3;
            *(((u8 *)actor) - (-controlOffset)) = zero;
            controlOffset += 5;
            *((u8 *)actor + controlOffset) = kind;
            actor->base.target = callback;
            offset = 0x2C8;
            byteZero = 0;
            *(u16 *)((u8 *)actor + offset) = zero;
            {
                u32 *field = &actor->base.pendingControlFlags;
                mask = ~1;
                *field = *field & mask;
            }
            actor->base.pendingControl = zero;
            {
                u32 state = 3;
                actor->base.state2A8 = byteZero;
                actor->base.phase2A9 = byteZero;
                {
                    u32 behOff = 0x2AA;
                    u8 *bq;
                    bq = (u8 *)actor + behOff;
                    *bq = state;
                }
                actor->base.counter2AC = zero;
                actor->base.field2B1 = one;
            }
        }
    }
}
