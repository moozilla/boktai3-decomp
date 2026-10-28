// MATCHING_CONCESSION: register-pinning
#include "scene_actor_control_phase.h"
#include "player.h"
void SceneActorControl_ResetCollisionPhase(struct SceneActorRegistryResetView *);
void SceneActorControl_StartPositionCurve(struct SceneActorRegistryResetView *, struct SceneVec *);
static inline s32 Quarter(s32 v)
{
    return v >= 0 ? v >> 2 : -(-v >> 2);
}
static inline s32 Clamp(s32 v)
{
    if (v > 200)
        return 200;
    if (v < -200)
        return -200;
    return v;
}
static inline u8 AtRest(s32 x, s32 z)
{
    if (!x && !z)
        return 1;
    return 0;
}
static inline u8 Test(u32 value, u32 mask)
{
    if (value & mask)
        return 1;
    return 0;
}
static inline void SetPhase(struct SceneActorRegistryResetView *actor, u8 state, u8 phase,
                            u8 behavior)
{
    actor->state2A8 = state;
    actor->phase2A9 = phase;
    actor->behavior2AA = behavior;
    actor->counter2AC = 0;
    actor->field2B1 = 1;
}
void SceneActorControl_ConvergeSavedMotion(struct SceneActorRegistryResetView *input)
{
    struct SceneActorRegistryResetView *actor = input;
    struct SceneActorControlState *control = SceneActorControl_GetState(actor);
    s32 x;
    s32 z;
    s16 *priorX;
    s16 *priorZ;
    if (SceneActorControl_TakeFlag(&actor->field2B2)) {
        {
            register u32 mask asm("r0") = 0x20;
            u16 *flags = &actor->flags2D4;
            *flags = mask | *flags;
        }
        SceneActorAnimation_SelectDirectionalEntry((void *)actor, 3);
        {
            u32 offset = 0x188;
            struct ContactCollider *collider = (void *)((u8 *)actor + offset);
            collider->flags |= 4;
        }
    }
    {
        u32 mask = 0x100000;
        u32 offset = 0x2D0;
        u32 *flags = (void *)((u8 *)actor + offset);
        *flags |= mask;
    }
    {
        u32 offset = 0x310;
        s32 current = *(s16 *)((u8 *)actor + offset);
        u32 priorOffset;
        s16 *field;
        s32 difference;
        priorOffset = 0x318;
        field = (void *)((u8 *)actor + priorOffset);
        difference = current - *field;
        priorX = field;
        x = Quarter(difference);
    }
    {
        register u32 offset asm("r4") = 0x314;
        s32 current = *(s16 *)((u8 *)actor + offset);
        register s16 *field asm("r1");
        s32 difference;
        offset += 8;
        asm("" : "+r"(offset));
        field = (void *)((u8 *)actor + offset);
        difference = current - *field;
        priorZ = field;
        z = Quarter(difference);
    }
    x = Clamp(x);
    z = Clamp(z);
    actor->motion.x += x;
    actor->motion.z += z;
    *priorX += x;
    *priorZ += z;
    if (AtRest(x, z)) {
        u32 ready;
        {
            u32 offset = 0x2B0;
            u8 *field = (u8 *)actor + offset;
            ready = 1;
            *field = ready;
        }
        {
            u32 flagsOffset = 0x2D4;
            u16 *flags = (void *)((u8 *)actor + flagsOffset);
            u32 mask = ready - 34;
            u32 stateTest;
            u8 *packet;
            u32 loaded;
            loaded = *flags;
            mask &= loaded;
            stateTest = 0;
            *flags = mask;
            {
                u32 *flags = &actor->flags2CC;
                u32 mask = ~0x80;
                *flags &= mask;
            }
            *((u8 *)control + 0x3E2) = stateTest;
            packet = (u8 *)actor + 0xDC;
            {
                u32 mask = 8;
                stateTest = Test(packet[15], mask);
            }
            if (stateTest) {
                register u32 zeroWord asm("r5");
                u32 mask = 0x100;
                register u32 loaded asm("r1") = *flags;
                stateTest = 0;
                zeroWord = 0;
                *flags = mask | loaded;
                SceneActorControl_StartPositionCurve(actor, &gPlayers[0]->position);
                {
                    u32 phase = 2;
                    register u32 offset asm("r2") = 0x2A8;
                    register u32 phaseOffset asm("r3");
                    u32 behaviorOffset;
                    u8 *field = (u8 *)actor + offset;
                    *field = stateTest;
                    phaseOffset = 0x2A9;
                    *((u8 *)actor + phaseOffset) = phase;
                    behaviorOffset = 0x2AA;
                    *((u8 *)actor + behaviorOffset) = stateTest;
                    offset += 4;
                    asm("" : "+r"(offset));
                    {
                        register u32 *field asm("r0") = (void *)((u8 *)actor + offset);
                        *field = zeroWord;
                    }
                    phaseOffset += 8;
                    {
                        register u8 *field asm("r1") = (u8 *)actor + phaseOffset;
                        *field = 1;
                    }
                }
                SceneActorControl_ResetCollisionPhase(actor);
            } else {
                register u32 mask asm("r0") = 8;
                packet[15] = mask | packet[15];
                mask = 0x100;
                {
                    register u32 loaded asm("r1") = *flags;
                    register u32 zero asm("r2") = 0;
                    *flags = mask | loaded;
                    {
                        u32 behavior = 3;
                        register u32 offset asm("r3") = 0x2A8;
                        *((u8 *)actor + offset) = zero;
                        offset++;
                        *((u8 *)actor + offset) = zero;
                        {
                            u32 behaviorOffset = 0x2AA;
                            *((u8 *)actor + behaviorOffset) = behavior;
                        }
                        offset += 3;
                        {
                            register u32 *field asm("r0") = (void *)((u8 *)actor + offset);
                            *field = stateTest;
                        }
                        {
                            register u32 readyOffset asm("r4") = 0x2B1;
                            register u8 *field asm("r1") = (u8 *)actor + readyOffset;
                            asm("" : "+r"(field) : "r"(readyOffset));
                            *field = 1;
                        }
                    }
                }
                SceneActorControl_ResetCollisionPhase(actor);
            }
        }
    }
}
