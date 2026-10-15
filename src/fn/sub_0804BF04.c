// MATCHING_CONCESSION: register-pinning
#ifdef NONMATCHING
// fuzzy: 93.5%
/* Complete body; remaining differences are scratch-register allocation. */
#include "tether_actor.h"
void TetherActor_SetState(struct TetherActor *, u32);
s32 TetherActor_SetEntry(struct TetherActor *, u32);
s32 TetherActor_SetHeadingEntry(struct TetherActor *, u32);
void TetherActor_UpdateShake(struct TetherActor *);
u32 TetherActor_TestStepDelta(void *, void *, u32);
u32 TetherActor_TestNearDelta(s32, s32);
void sub_0821FAD0(void *, void *);
static inline s32 scale(s32 value)
{
    if (value >= 0)
        return value >> 12;
    return -(-value >> 12);
}
void TetherActor_UpdateRoamingState(struct TetherActor *actor)
{
    struct ContactPosition *position;
    struct ContactPackedVector delta;
    s32 activeMask;
    s32 headingMask;
    register u32 chargeOffset asm("r2");
    if (actor->state != 3)
        TetherActor_SetState(actor, 3);
    if (gUnk_030053F4 & 512)
        return;
    chargeOffset = 0x384;
    if (*(u16 *)((u8 *)actor + chargeOffset)) {
        {
            register u32 glowOffset asm("r3") = 0x38A;
            asm("" : "+r"(glowOffset));
            {
                register u16 *glow asm("r0") = (u16 *)((u8 *)actor + glowOffset);
                *glow = 0;
            }
        }
        chargeOffset += 0x9C;
        {
            u16 *frame = (u16 *)((u8 *)actor + chargeOffset);
            *frame = 0;
        }
        asm("" : : "r"(chargeOffset));
        TetherActor_UpdateShake(actor);
        position = &actor->position;
    } else {
        u16 *frame;
        activeMask = 1;
        actor->glowMode = activeMask;
        {
            u32 currentFrame = actor->roamFrame;
            position = &actor->position;
            if (currentFrame > 90) {
                s32 dx;
                s32 dz;
                register s16 *z asm("r10");
                {
                    register s32 targetX asm("r1") = actor->interactionProbe.target.x;
                    s32 worldX = position->x;
                    targetX -= worldX;
                    asm("" : "+r"(targetX));
                    dx = targetX;
                }
                headingMask = 7;
                {
                    register s32 targetZ asm("r2") = actor->interactionProbe.target.z;
                    s16 *worldZ = &actor->position.z;
                    s32 value = *worldZ;
                    targetZ -= value;
                    asm("" : "+r"(targetZ));
                    dz = targetZ;
                    z = worldZ;
                }
                if (dx || dz) {
                    register u32 angle asm("r5") = Math_Atan2(dx, dz);
                    delta.x = scale(gUnk_086149C4[(angle + 64) & 255] * 3);
                    delta.y = 0;
                    {
                        s32 stepZ = scale(gUnk_086149C4[angle & 255] * 3);
                        delta.z = stepZ;
                    }
                    {
                        u32 accepted = TetherActor_TestStepDelta(position, &delta, 0x841);
                        if (accepted) {
                            position->x += delta.x;
                            {
                                u32 value = (u16)delta.z;
                                register s16 *source asm("r2") = z;
                                register u32 current asm("r2") = *(u16 *)source;
                                register s16 *destination asm("r3");
                                value += current;
                                destination = z;
                                *(u16 *)destination = value;
                            }
                            actor->moving = 1;
                        } else
                            actor->moving = accepted;
                    }
                    {
                        register s32 sector asm("r0") = (u16)angle;
                        u32 mask = headingMask;
                        asm("" : "+r"(sector));
                        sector += 112;
                        sector >>= 5;
                        mask &= sector;
                        actor->heading = mask;
                    }
                }
                if (!(actor->interactionProbe.flags & activeMask) &&
                    TetherActor_TestNearDelta(dx, dz) && actor->interactionScriptId)
                    Script_Call(actor->interactionScriptId, 0);
            }
        }
        frame = &actor->roamFrame;
        if (++*frame > 119)
            *frame = 0;
        if (actor->kind == 5 && actor->moving)
            TetherActor_SetHeadingEntry(actor, 36);
        else
            TetherActor_SetEntry(actor, 0);
    }
    sub_0821FAD0(&actor->interactionProbe, position);
}

#else
#include "global.h"
INCLUDE_ASM("asm/nonmatching", TetherActor_UpdateRoamingState);
#endif
