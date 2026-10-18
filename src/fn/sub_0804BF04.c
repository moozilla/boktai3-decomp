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
void NavMesh_UpdateProbe(void *, void *);
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
    u32 chargeOffset;
    if (actor->state != 3)
        TetherActor_SetState(actor, 3);
    if (gUnk_030053F4 & 512)
        return;
    chargeOffset = 0x384;
    if (*(u16 *)((u8 *)actor + chargeOffset)) {
        {
            u32 glowOffset = 0x38A;
            {
                u16 *glow = (u16 *)((u8 *)actor + glowOffset);
                *glow = 0;
            }
        }
        chargeOffset += 0x9C;
        {
            u16 *frame = (u16 *)((u8 *)actor + chargeOffset);
            *frame = 0;
        }
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
                s16 *z;
                {
                    s32 targetX = actor->interactionProbe.target.x;
                    s32 worldX = position->x;
                    targetX -= worldX;
                    dx = targetX;
                }
                headingMask = 7;
                {
                    s32 targetZ = actor->interactionProbe.target.z;
                    s16 *worldZ = &actor->position.z;
                    s32 value = *worldZ;
                    targetZ -= value;
                    dz = targetZ;
                    z = worldZ;
                }
                if (dx || dz) {
                    u32 angle = Math_Atan2(dx, dz);
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
                                s16 *source = z;
                                u32 current = *(u16 *)source;
                                s16 *destination;
                                value += current;
                                destination = z;
                                *(u16 *)destination = value;
                            }
                            actor->moving = 1;
                        } else
                            actor->moving = accepted;
                    }
                    {
                        s32 sector = (u16)angle;
                        u32 mask = headingMask;
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
    NavMesh_UpdateProbe(&actor->interactionProbe, position);
}

#else
#include "global.h"
INCLUDE_ASM("asm/nonmatching", TetherActor_UpdateRoamingState);
#endif
