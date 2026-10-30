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
void Bee_DisableMovement(void *);
// confidence=low: eases saved motion before returning to idle.
void Bee_UpdateMovementStop(struct SceneActorRegistryResetView *input)
{
    struct SceneActorRegistryResetView *actor = input;
    u8 *body = actor->context;
    s32 x;
    s32 z;
    s16 *priorX;
    s16 *priorZ;
    u16 *animationFlags;
    register u32 previousOffset asm("r4");
    register u32 entryOffset asm("r2") = 0x2B2;
    asm("" : "+r"(entryOffset));
    if (SceneActorControl_TakeFlag((u8 *)actor + entryOffset)) {
        {
            register u32 mask asm("r0") = 0x20;
            u32 offset = 0x2D4;
            animationFlags = (void *)((u8 *)actor + offset);
            *animationFlags = mask | *animationFlags;
        }
        SceneActorAnimation_SelectDirectionalEntry((void *)actor, 3);
        {
            u32 mask = ~0x100;
            *animationFlags = mask & *animationFlags;
        }
    }
    {
        u32 mask = 0x100000;
        u32 *flags;
        previousOffset = 0x2D0;
        flags = (void *)((u8 *)actor + previousOffset);
        *flags |= mask;
    }
    {
        u32 offset = 0x310;
        s32 current;
        s16 *currentField;
        u32 priorOffset;
        s16 *field;
        s32 difference;
        currentField = (void *)((u8 *)actor + offset);
        current = *currentField;
        previousOffset += 0x48;
        priorOffset = previousOffset;
        field = (void *)((u8 *)actor + priorOffset);
        difference = current - *field;
        asm("" : : "r"(previousOffset));
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
    actor->body.motion.x += x;
    actor->body.motion.z += z;
    *priorX += x;
    *priorZ += z;
    if (AtRest(x, z)) {
        u32 zeroWord;
        u32 zeroByte;
        {
            u8 *state = &actor->field2B0;
            zeroWord = 0;
            asm("" : "+r"(zeroWord));
            *state = 1;
        }
        {
            u32 offset = 0x2D4;
            u16 *p;
            u32 mask;
            u32 value;
            p = (void *)((u8 *)actor + offset);
            mask = ~0x20;
            asm("" : "+r"(mask));
            value = mask & *p;
            zeroByte = 0;
            *p = value;
        }
        {
            u32 offset = 0x2CC;
            u32 *p;
            u32 mask;
            p = (void *)((u8 *)actor + offset);
            mask = ~0x80;
            *p &= mask;
        }
        Bee_DisableMovement(actor);
        {
            u32 phase = 2;
            register u32 offset asm("r2") = 0x2A8;
            register u32 phaseOffset asm("r3");
            *((u8 *)actor + offset) = zeroByte;
            phaseOffset = 0x2A9;
            *((u8 *)actor + phaseOffset) = phase;
            actor->behavior2AA = zeroByte;
            offset += 4;
            asm("" : "+r"(offset));
            {
                register u32 *p asm("r0") = (void *)((u8 *)actor + offset);
                *p = zeroWord;
            }
            phaseOffset += 8;
            {
                register u8 *p asm("r0") = (u8 *)actor + phaseOffset;
                *p = 1;
            }
        }
        {
            u32 mask = 8;
            u32 *flags = (void *)(body + 0x3D4);
            *flags |= mask;
        }
        body[0x42A] = 60;
    }
}
