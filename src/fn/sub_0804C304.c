#include "tether_actor.h"
// MATCHING_CONCESSION: register-pinning
void TetherActor_SetState(struct TetherActor *, u32);
void TetherActor_SetEntry(struct TetherActor *, u32);
void TetherActor_SelectInteractionState(struct TetherActor *);
void TetherActor_EmitRandomEffect(struct TetherActor *);
void sub_08066F90(s32);
s32 Mod(s32, s32);
void TetherActor_UpdateRecoveryState(struct TetherActor *actor)
{
    u32 frozen;
    register u32 rawTimer asm("r0");
    u32 highTimer;
    register u16 *timer asm("r8");
    register u32 counterOffset asm("r6");
    if (actor->state != 2)
        TetherActor_SetState(actor, 2);
    frozen = gUnk_030053F4 & 512;
    if (frozen)
        return;
    rawTimer = actor->recoveryTimer;
    highTimer = rawTimer << 16;
    if ((highTimer >> 16) > 64) {
        struct ContactCollider *collider;
        register u32 colliderOffset asm("r6");
        actor->glowMode = 2;
        if (actor->parameter >= 0)
            sub_08066F90(actor->parameter);
        if (actor->transitionReason == 2) {
            actor->secondaryCollider.bounds.x = 256;
            {
                register u32 offset asm("r6") = 0x1A2;
                register u16 *height asm("r2");
                asm("" : "+r"(offset));
                height = (u16 *)((u8 *)actor + offset);
                *height = 32;
            }
            actor->secondaryCollider.bounds.z = 256;
            actor->transitionReason = 1;
        } else if (actor->transitionReason == 1) {
            actor->secondaryCollider.bounds.x = 128;
            actor->secondaryCollider.bounds.y = 32;
            actor->secondaryCollider.bounds.z = 128;
            actor->transitionReason = frozen;
        }
        colliderOffset = 0x184;
        collider = (struct ContactCollider *)((u8 *)actor + colliderOffset);
        Collider_SetPosition(collider, &actor->position, 0);
        Collider_Refresh(collider);
        {
            u32 offset = 0x400;
            timer = (u16 *)(offset + (u32)actor);
        }
        if ((*timer & 31) == 0) {
            gUnk_03005308 = (gUnk_03005308 + 1) & 1023;
            {
                u32 sample = Mod(((u16 *)RANDOM_TABLE_ADDR)[gUnk_03005308], 3);
                register u8 *heading asm("r2");
                colliderOffset += 0xDC;
                heading = (u8 *)actor + colliderOffset;
                *heading = (*heading + sample - 1) & 7;
            }
            TetherActor_SetEntry(actor, 0);
        }
        {
            const u16 *table;
            register const u16 *rawTable asm("r0");
            register u32 *randomIndex asm("r5");
            register u32 index asm("r1");
            register u32 mask asm("r3");
            register u32 amplitudeMask asm("r4");
            register u32 shakeOffset asm("r6");
            rawTable = (const u16 *)RANDOM_TABLE_ADDR;
            asm("" : "+r"(rawTable));
            table = rawTable;
            randomIndex = &gUnk_03005308;
            index = *randomIndex + 1;
            mask = 1023;
            index &= mask;
            {
                register u32 sample asm("r0") = table[index];
                s16 *shake;
                amplitudeMask = 63;
                sample &= amplitudeMask;
                sample -= 31;
                shakeOffset = 0x37E;
                shake = (s16 *)((u8 *)actor + shakeOffset);
                *shake = sample;
            }
            index = (index + 1) & mask;
            *randomIndex = index;
            actor->shakeZ = (table[index] & amplitudeMask) - 31;
            asm("" : : "r"(shakeOffset));
        }
        {
            register u16 *frame asm("r6") = timer;
            if ((*frame & 7) == 0)
                TetherActor_EmitRandomEffect(actor);
        }
        {
            u32 chargeHigh = (u32)actor->charge << 16;
            if (chargeHigh) {
                s32 reduction = chargeHigh >> 19;
                register u16 *current asm("r6") = timer;
                u32 remaining = *current;
                if ((s32)remaining < reduction + 64) {
                    *current = 64;
                } else {
                    u16 *destination;
                    register u32 value asm("r0") = remaining - reduction;
                    destination = timer;
                    *destination = value;
                }
                actor->charge = 0;
                {
                    register u32 readyOffset asm("r6") = 0x384;
                    *(u16 *)((u8 *)actor + readyOffset) = 0;
                    asm("" : : "r"(readyOffset));
                }
            }
        }
    } else {
        s32 amplitude = highTimer >> 17;
        register const u16 *table asm("r8");
        u32 *randomIndex;
        u32 randomMask;
        if (amplitude < 2)
            amplitude = 2;
        table = (const u16 *)RANDOM_TABLE_ADDR;
        {
            register u32 *slot asm("r1") = &gUnk_03005308;
            u32 index;
            u32 mask;
            randomIndex = slot;
            asm("" : : "r"(randomIndex));
            index = *slot + 1;
            mask = 1023;
            randomMask = mask;
            index &= mask;
            *slot = index;
            actor->shakeX = Mod(table[index], amplitude) - (amplitude >> 1);
        }
        {
            u32 *slot = randomIndex;
            u32 index = *slot + 1;
            u32 mask = randomMask;
            index &= mask;
            *slot = index;
            actor->shakeZ = Mod(table[index], amplitude) - (amplitude >> 1);
        }
        actor->glowMode = frozen;
        actor->charge = frozen;
        actor->chargeReady = frozen;
        {
            u32 soundFrame = actor->lockSoundFrame + 1;
            register u32 truncation asm("r6");
            u32 mask;
            actor->lockSoundFrame = soundFrame;
            truncation = 65535;
            asm("" : "+r"(truncation));
            mask = truncation;
            asm("" : "+r"(mask));
            soundFrame &= mask;
            mask = 7;
            soundFrame &= mask;
            if (soundFrame == 0) {
                u32 *slot = randomIndex;
                u32 index = *slot + 1;
                u32 mask = randomMask;
                u32 sample;
                index &= mask;
                *slot = index;
                sample = table[index];
                sample &= 1;
                if (sample)
                    Sound_PlaySong(0x2B9);
            }
        }
    }
    counterOffset = 0x400;
    asm("" : "+r"(counterOffset));
    {
        register u16 *counter asm("r1") = (u16 *)((u8 *)actor + counterOffset);
        if (--*counter == 0) {
            if (actor->unknown1B == 1) {
                TetherActor_SetState(actor, 16);
                {
                    u32 offset = 0x46A;
                    u16 *directed = (u16 *)((u8 *)actor + offset);
                    *directed = 30;
                }
            } else
                TetherActor_SelectInteractionState(actor);
        }
    }
}
