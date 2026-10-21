// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "scene_actor_reaction.h"

void Ivy_UpdateActiveState_080E98B4(void *);
void SceneActorReaction_PlayEntry17(struct SceneActorRegistryResetView *actor)
{
    if (SceneActorReaction_TakePrepared(actor)) {
        actor->colliderA.flags = (actor->colliderA.flags & 0xFFFB) | 8;
        {
            u16 *field = &actor->flags2C0;
            u32 mask = ~0x800;
            *field = mask & *field;
        }
        SceneActorAnimation_SelectDirectionalEntry((void *)actor, 17);
    }
    {
        u32 controlOffset = 0x2B3;
        asm("" : "+r"(controlOffset));
        if (*((u8 *)actor + controlOffset)) {
            void *callback = Ivy_UpdateActiveState_080E98B4;
            u8 kind = 34;
            u8 *prepared = &actor->field2B2;
            u32 zero = 0;
            u8 one = 1;
            u32 offset;
            u32 mask;
            u8 byteZero;
            *prepared = one;
            controlOffset -= 3;
            *((u8 *)actor + controlOffset) = zero;
            controlOffset += 5;
            *((u8 *)actor + controlOffset) = kind;
            actor->target = callback;
            offset = 0x2C8;
            byteZero = 0;
            *(u16 *)((u8 *)actor + offset) = zero;
            {
                u32 *field = &actor->flags114;
                mask = ~1;
                *field = *field & mask;
            }
            actor->word104 = zero;
            {
                u32 state = 3;
                actor->state2A8 = byteZero;
                actor->phase2A9 = byteZero;
                {
                    register u32 behOff asm("r4") = 0x2AA;
                    register u8 *bq asm("r0");
                    asm("" : "+r"(behOff));
                    bq = (u8 *)actor + behOff;
                    *bq = state;
                }
                actor->counter2AC = zero;
                actor->field2B1 = one;
            }
        }
    }
}
