// MATCHING_CONCESSION: register-pinning
#include "scene_actor_reaction.h"
#define SceneActorReaction_ResumeAfterAnimation sub_08081B9C
void sub_08076034(void *, s32);
void sub_080771C4(void *);
static inline u8 TakePrepared(struct SceneActorRegistryResetView *actor)
{
    register u32 offset asm("r2") = 0x2B2;
    u8 *flag;
    asm("" : : "r"(offset));
    flag = (u8 *)actor + offset;
    if (*flag) {
        *flag = 0;
        return 1;
    }
    return 0;
}
void SceneActorReaction_ResumeAfterAnimation(struct SceneActorRegistryResetView *actor,
                                             s32 unusedFrame)
{
    u8 *group = (u8 *)actor + 0x3D4;
    u32 phaseOffset;
    if (TakePrepared(actor)) {
        sub_08076034(actor, 7);
        sub_080771C4(actor);
        {
            register u32 offset asm("r3") = 0x2D4;
            register u16 *flags asm("r0");
            u32 mask;
            asm("" : : "r"(offset));
            flags = (u16 *)((u8 *)actor + offset);
            mask = ~2;
            {
                register u32 value asm("r2") = *flags;
                *flags = mask & value;
            }
        }
    }
    {
        register u32 completedOffset asm("r1") = 0x2B3;
        register u8 *completed asm("r0");
        completed = (u8 *)actor + completedOffset;
        if (*completed) {
            u32 zeroWord;
            u32 zeroByte;
            u32 one;
            u16 *valueAA = &actor->fieldAA;
            zeroByte = 0;
            asm("" : "+r"(zeroByte));
            zeroWord = 0;
            *valueAA = zeroWord;
            {
                register u32 offset asm("r2") = 0x2B0;
                register u8 *field asm("r0");
                asm("" : : "r"(offset));
                field = (u8 *)actor + offset;
                one = 1;
                *field = one;
            }
            {
                register u32 mask asm("r2") = 0x10;
                register u32 offset asm("r3") = 0x2CC;
                register u32 *flags asm("r1");
                asm("" : : "r"(offset));
                flags = (u32 *)((u8 *)actor + offset);
                *flags |= mask;
            }
            sub_08076034(actor, 0);
            {
                register u32 state asm("r1") = 3;
                u32 stateOffset = 0x2A8;
                register u8 *field asm("r0");
                field = (u8 *)actor + stateOffset;
                *field = state;
                phaseOffset = 0x2A9;
                {
                    field = (u8 *)actor + phaseOffset;
                }
                *field = one;
                {
                    register u32 offset asm("r1") = 0x2AA;
                    field = (u8 *)actor + offset;
                }
                *field = zeroByte;
                {
                    register u32 *counter asm("r0");
                    stateOffset += 4;
                    counter = (u32 *)((u8 *)actor + stateOffset);
                    *counter = zeroWord;
                }
                phaseOffset += 8;
                field = (u8 *)actor + phaseOffset;
                *field = one;
            }
            {
                register void *control asm("r2") = *(void **)(group + 0x210);
                register u32 kind asm("r1") = 15;
                {
                    u8 *field;
                    phaseOffset += 1;
                    field = (u8 *)actor + phaseOffset;
                    *field = one;
                }
                {
                    u8 *field;
                    phaseOffset -= 2;
                    field = (u8 *)actor + phaseOffset;
                    *field = zeroByte;
                }
                {
                    register u8 *field asm("r0");
                    phaseOffset += 5;
                    field = (u8 *)actor + phaseOffset;
                    *field = kind;
                }
                actor->target = control;
                {
                    register u32 offset asm("r2") = 0x2C8;
                    register u16 *field asm("r0");
                    asm("" : : "r"(offset));
                    field = (u16 *)((u8 *)actor + offset);
                    *field = zeroWord;
                }
                {
                    register u32 offset asm("r3") = 0x114;
                    register u32 *flags asm("r2");
                    u32 mask;
                    asm("" : : "r"(offset));
                    flags = (u32 *)((u8 *)actor + offset);
                    mask = ~1;
                    *flags &= mask;
                }
                {
                    register u32 offset asm("r1") = 0x104;
                    register u32 *field asm("r0");
                    field = (u32 *)((u8 *)actor + offset);
                    *field = zeroWord;
                }
            }
        }
    }
}
