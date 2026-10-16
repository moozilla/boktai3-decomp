#include "tether_actor.h"
// MATCHING_CONCESSION: register-pinning
void TetherActor_SetState(struct TetherActor *, u32);
void TetherActor_RemoveAuxiliarySprite(struct TetherActor *);
void TetherActor_EmitRingEffects(struct TetherActor *);
void *ActorAttachment_UpdateFromDescriptor(void *, struct TetherBodyConfig *, u32, u32, u16 *, u32,
                                           u32, u32);
void TetherActor_UpdateBounceReturn(struct TetherActor *actor)
{
    u16 *frame;
    u32 zero;
    register u32 frameValue asm("r0");
    if (actor->state != 8)
        TetherActor_SetState(actor, 8);
    {
        u16 *glow = &actor->glowMode;
        zero = 0;
        *glow = zero;
    }
    frame = &actor->stateFrame;
    frameValue = *frame;
    if (!frameValue) {
        u16 *height;
        actor->shakeX = zero;
        actor->shakeZ = zero;
        height = &actor->displayOffsetY;
        if (*height <= 30) {
            TetherActor_RemoveAuxiliarySprite(actor);
            *height = zero;
            *frame = 1;
        } else
            *height -= 30;
        return;
    }
    {
        register u32 incremented asm("r1") = frameValue + 1;
        u32 current;
        *frame = incremented;
        current = (u16)incremented;
        if (current <= 7) {
            current = (u16)incremented;
            if (current <= 3)
                actor->displayOffsetY = incremented << 4;
            else {
                u32 height = (7 - incremented) << 4;
                register u32 heightOffset asm("r6") = 0x380;
                register u16 *destination asm("r1");
                asm("" : "+r"(heightOffset));
                destination = (u16 *)((u8 *)actor + heightOffset);
                *destination = height;
            }
        } else
            actor->displayOffsetY = zero;
    }
    {
        u16 *counter;
        u32 offset = 0x256;
        register u16 *counterAddress asm("r2");
        asm("" : "+r"(offset));
        counterAddress = (u16 *)(offset + (u32)actor);
        counter = counterAddress;
        if (*counterAddress <= 11) {
            register const u16 *rawTable asm("r6") = (const u16 *)RANDOM_TABLE_ADDR;
            register const u16 *table asm("r12");
            u32 *randomIndex;
            register u32 index asm("r1");
            u32 mask;
            register u32 amplitudeMask asm("r4");
            register u32 shakeOffset asm("r6");
            asm("" : "+r"(rawTable));
            table = rawTable;
            randomIndex = &gUnk_03005308;
            index = *randomIndex + 1;
            mask = 1023;
            index &= mask;
            {
                u32 sample = table[index];
                s16 *shake;
                amplitudeMask = 31;
                sample &= amplitudeMask;
                sample -= 16;
                shakeOffset = 0x37E;
                shake = (s16 *)((u8 *)actor + shakeOffset);
                *shake = sample;
            }
            index = (index + 1) & mask;
            *randomIndex = index;
            actor->shakeZ = (table[index] & amplitudeMask) - 16;
            asm("" : : "r"(shakeOffset));
        }
        {
            register u16 *frameAlias asm("r6") = counter;
            asm("" : "+r"(frameAlias));
            if (*frameAlias == 2)
                TetherActor_EmitRingEffects(actor);
        }
        {
            u16 *frameAlias = counter;
            if (*frameAlias > 29) {
                register u16 *charge asm("r2");
                void **handle;
                TetherActor_SetState(actor, 0);
                {
                    register u32 readyOffset asm("r2") = 0x384;
                    register u16 *ready asm("r1");
                    ready = (u16 *)((u8 *)actor + readyOffset);
                    *ready = 1;
                }
                {
                    register u32 chargeOffset asm("r6") = 0x386;
                    u32 value;
                    asm("" : "+r"(chargeOffset));
                    charge = (u16 *)((u8 *)actor + chargeOffset);
                    value = 287;
                    *charge = value;
                    handle = &actor->pendingEffectHandle;
                    *handle = ActorAttachment_UpdateFromDescriptor(*handle, &actor->bodyConfig, 1,
                                                                   0, charge, value, 250, 290);
                }
            }
        }
    }
}
