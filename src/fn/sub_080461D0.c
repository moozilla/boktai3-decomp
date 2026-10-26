#include "contact_sequence_actor.h"
u32 ContactSequenceActor_TakeEntryFlag(struct ContactSequenceActor *);
void ContactSequenceActor_SetFlags(struct ContactSequenceActor *, u32);
void ContactSequenceActor_Deactivate(struct ContactSequenceOwner *, struct ContactSequenceActor *);
s32 TransientSpriteEffects_Spawn(u32, u32, const struct ContactPosition *,
                                 const struct ContactPosition *, u32, u32);
void sub_080148CC(u32, u32, void *, u32, u32, u32, u32, u32, u32, u32, u32);
struct ContactSequenceScriptArgs {
    u32 count : 16;
    u32 unused : 16;
    s32 *values;
};
#ifdef NONMATCHING
// fuzzy: 84.4%
// fix.py best (sweep after the inlined-insn fix)
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
    u32 xzMask;
    if (ContactSequenceActor_TakeEntryFlag(actor)) {
        table = (const u16 *)0x0203B400;
        {
            u16 randomIndex;
            u32 sample;
            u32 smallMask;
            u32 lowMask;
            u32 sampleY;
            u32 yMask;
            u32 *counter;
            u32 sampleZ;
            u32 zMask;
            randomIndex = gRandomIndex + 1;
            randomMask = 1023;
            randomIndex &= randomMask;
            sample = table[randomIndex];
            smallMask = 63;
            xzMask = smallMask;
            lowMask = xzMask;
            scatter.bits.x = sample & lowMask;
            randomIndex = (randomIndex + 1) & randomMask;
            sampleY = table[randomIndex];
            yMask = 127;
            sampleY &= yMask;
            scatter.bits.y = sampleY;
            randomIndex = (randomIndex + 1) & randomMask;
            counter = &gRandomIndex;
            *counter = randomIndex;
            sampleZ = table[randomIndex];
            zMask = xzMask;
            scatter.bits.z = sampleZ & zMask;
        }
        scatterPosition = &scatter.position;
        {
            u32 x = (u16)actor->position.x;
            u32 dx = (u16)scatterPosition->x;
            u32 y;
            u32 sourceY;
            struct ContactPosition *lowPosition;
            u32 positionOffset;
            u32 sourceZ;
            u32 dz;
            x += dx;
            scatterPosition->x = x;
            y = (u16)scatterPosition->y;
            positionOffset = 0x10;
            lowPosition = (struct ContactPosition *)(positionOffset + ((u32)actor));
            position = lowPosition;
            sourceY = (u16)actor->position.y;
            y += sourceY;
            scatterPosition->y = y;
            sourceZ = (u16)lowPosition->z;
            dz = (u16)scatterPosition->z;
            sourceZ += dz;
            scatterPosition->z = sourceZ;
        }
        TransientSpriteEffects_Spawn(0, 0x1004, scatterPosition, 0, 256, 256);
        {
            u8 randomIndex;
            u32 sampleX;
            u32 lowMask;
            u32 sampleY;
            u32 yMask;
            u32 *counter;
            u32 sampleZ;
            u32 sourceX;
            struct ContactPosition *lowPosition;
            u32 sourceY;
            u32 zMask;
            u32 sourceZ;
            randomIndex = (gRandomIndex + 1) & randomMask;
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
            counter = &gRandomIndex;
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
            sampleZ &= zMask;
            sourceZ = (u16)lowPosition->z;
            sampleZ += sourceZ;
            scatterPosition->z = sampleZ;
        }
        TransientSpriteEffects_Spawn(0, 0x1004, scatterPosition, 0, 256, 256);
        sub_080148CC(4, 14, position, 64, 256, 12, 8, 8, 8, 30, 30);
        {
            s32 health = actor->health;
            if ((health <= 0) && parent->script) {
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
INCLUDE_ASM("asm/nonmatching", ContactSequenceActor_UpdateDying);
#endif
