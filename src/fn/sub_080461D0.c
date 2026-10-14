// MATCHING_CONCESSION: register-pinning
#include "contact_sequence_actor.h"
#define ContactSequenceActor_UpdateDying   sub_080461D0
#define ContactSequenceActor_TakeEntryFlag sub_08045D3C
#define ContactSequenceActor_SetFlags      sub_08045D18
#define ContactSequenceActor_Deactivate    sub_08045F14
u32 ContactSequenceActor_TakeEntryFlag(struct ContactSequenceActor *);
void ContactSequenceActor_SetFlags(struct ContactSequenceActor *, u32);
void ContactSequenceActor_Deactivate(struct ContactSequenceOwner *, struct ContactSequenceActor *);
void sub_080474E4(u32, u32, void *, u32, u32, u32);
void sub_080148CC(u32, u32, void *, u32, u32, u32, u32, u32, u32, u32, u32);
struct ContactSequenceScriptArgs {
    u32 count : 16;
    u32 unused : 16;
    s32 *values;
};
#ifdef NONMATCHING
// fuzzy: 97.0%
// Remaining differences: signed-field scratch registers and the script-count mask reload.
void ContactSequenceActor_UpdateDying(struct ContactSequenceOwner *parent,
                                      struct ContactSequenceActor *actor)
{
    union {
        struct ContactPackedVector bits;
        struct ContactPosition position;
    } scatter;
    struct ContactPosition *position;
    struct ContactPosition *scatterPosition;
    s32 coordinates[3];
    struct ContactSequenceScriptArgs args;
    const u16 *table;
    u32 randomMask;
    register u32 xzMask asm("r10");
    if (ContactSequenceActor_TakeEntryFlag(actor)) {
        table = (const u16 *)RANDOM_TABLE_ADDR;
        {
            register u32 randomIndex asm("r1");
            u32 sample;
            register u32 smallMask asm("r3");
            register u32 lowMask asm("r2");
            u32 sampleY;
            u32 yMask;
            register u32 *counter asm("r2");
            u32 sampleZ;
            register u32 zMask asm("r3");
            randomIndex = gUnk_03005308 + 1;
            randomMask = 1023;
            asm("" : "+r"(randomMask));
            randomIndex &= randomMask;
            sample = table[randomIndex];
            smallMask = 63;
            asm("" : : "r"(smallMask));
            xzMask = smallMask;
            lowMask = xzMask;
            asm("" : : "r"(lowMask));
            scatter.bits.x = sample & lowMask;
            randomIndex = (randomIndex + 1) & randomMask;
            sampleY = table[randomIndex];
            yMask = 127;
            sampleY &= yMask;
            scatter.bits.y = sampleY;
            randomIndex = (randomIndex + 1) & randomMask;
            counter = &gUnk_03005308;
            asm("" : "+r"(counter));
            *counter = randomIndex;
            sampleZ = table[randomIndex];
            zMask = xzMask;
            asm("" : : "r"(zMask));
            scatter.bits.z = sampleZ & zMask;
        }
        scatterPosition = &scatter.position;
        asm("" : "+r"(scatterPosition));
        {
            u32 x = (u16)actor->position.x;
            register u32 dx asm("r3") = (u16)scatterPosition->x;
            u32 y;
            register u32 sourceY asm("r2");
            register struct ContactPosition *lowPosition asm("r1");
            u32 positionOffset;
            register u32 sourceZ asm("r0");
            register u32 dz asm("r3");
            x += dx;
            scatterPosition->x = x;
            y = (u16)scatterPosition->y;
            positionOffset = 0x10;
            asm("" : "+r"(positionOffset));
            lowPosition = (struct ContactPosition *)(positionOffset + (u32)actor);
            position = lowPosition;
            sourceY = (u16)actor->position.y;
            y += sourceY;
            scatterPosition->y = y;
            sourceZ = (u16)lowPosition->z;
            dz = (u16)scatterPosition->z;
            sourceZ += dz;
            scatterPosition->z = sourceZ;
        }
        sub_080474E4(0, 0x1004, scatterPosition, 0, 256, 256);
        {
            register u32 randomIndex asm("r0");
            u32 sampleX;
            register u32 lowMask asm("r2");
            u32 sampleY;
            u32 yMask;
            register u32 *counter asm("r1");
            u32 sampleZ;
            u32 sourceX;
            register struct ContactPosition *lowPosition asm("r3");
            u32 sourceY;
            u32 zMask;
            register u32 sourceZ asm("r2");
            randomIndex = (gUnk_03005308 + 1) & randomMask;
            sampleX = table[randomIndex];
            lowMask = xzMask;
            sampleX &= lowMask;
            scatterPosition->x = sampleX;
            randomIndex = (randomIndex + 1) & randomMask;
            sampleY = table[randomIndex];
            yMask = 127;
            sampleY &= yMask;
            scatterPosition->y = sampleY;
            randomIndex = (randomIndex + 1) & randomMask;
            counter = &gUnk_03005308;
            asm("" : "+r"(counter));
            *counter = randomIndex;
            sampleZ = table[randomIndex];
            sourceX = (u16)actor->position.x;
            sampleX += sourceX;
            scatterPosition->x = sampleX;
            lowPosition = position;
            sourceY = (u16)lowPosition->y;
            sampleY += sourceY;
            scatterPosition->y = sampleY;
            zMask = xzMask;
            asm("" : : "r"(zMask));
            sampleZ &= zMask;
            sourceZ = (u16)lowPosition->z;
            asm("" : "+r"(sourceZ));
            sampleZ += sourceZ;
            scatterPosition->z = sampleZ;
        }
        sub_080474E4(0, 0x1004, scatterPosition, 0, 256, 256);
        sub_080148CC(4, 14, position, 64, 256, 12, 8, 8, 8, 30, 30);
        {
            s32 health = actor->health;
            if (health <= 0 && parent->script) {
                coordinates[0] = actor->position.x;
                coordinates[1] = actor->position.y;
                coordinates[2] = actor->position.z;
                args.count = 3;
                args.values = coordinates;
                Script_Call(parent->script, &args);
            }
        }
        Sound_PlaySongChecked(0x3BA);
    }
    ContactSequenceActor_SetFlags(actor, 1);
    ContactSequenceActor_Deactivate(parent, actor);
}
#else
INCLUDE_ASM("asm/nonmatching", sub_080461D0);
#endif
