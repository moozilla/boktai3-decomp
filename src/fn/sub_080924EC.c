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
// confidence=low: eases saved motion before returning to idle; register allocation still differs.
#define Bee_UpdateMovementStop sub_080924EC
#ifdef NONMATCHING
// fuzzy: 86.5%
void Bee_UpdateMovementStop(struct SceneActorRegistryResetView *input)
{
    struct SceneActorRegistryResetView *actor = input;
    register u8 *body asm("r8")=actor->context;
    s32 x;
    s32 z;
    s16 *priorX;
    s16 *priorZ;
    asm("" : "+r"(body));
    if (SceneActorControl_TakeFlag(&actor->field2B2)) {
        {
            register u32 mask asm("r0") = 0x20;
            u16 *flags = &actor->flags2D4;
            *flags = mask | *flags;
        }
        SceneActorAnimation_SelectDirectionalEntry((void *)actor, 3);
        {
            u32 mask=~0x100;
            actor->flags2D4=mask & actor->flags2D4;
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
    if (AtRest(x,z)) {
      register u32 zeroWord asm("r6")=0;
      register u32 zeroByte asm("r4");
      asm("" : "+r"(zeroWord));
      actor->field2B0=1;
      {u16 *p=&actor->flags2D4; u32 mask=~0x20; *p=mask & *p; zeroByte=0; asm("" : "+r"(zeroByte));}
      {u32 *p=&actor->flags2CC; u32 mask=~0x80; *p &=mask;}
      Bee_DisableMovement(actor);
      {u32 phase=2; register u32 offset asm("r2")=0x2A8;
       register u32 phaseOffset asm("r3");
       *((u8 *)actor+offset)=zeroByte; phaseOffset=0x2A9; *((u8 *)actor+phaseOffset)=phase;
       actor->behavior2AA=zeroByte; offset+=4; asm("" : "+r"(offset));
       {register u32 *p asm("r0")=(void *)((u8 *)actor+offset); *p=zeroWord;}
       phaseOffset+=8; {register u8 *p asm("r0")=(u8 *)actor+phaseOffset; *p=1;}
      }
      {u32 mask=8; *(u32 *)(body+0x3D4) |=mask;}
      body[0x42A]=60;
    }
}

#else
INCLUDE_ASM("asm/nonmatching", sub_080924EC);
#endif
