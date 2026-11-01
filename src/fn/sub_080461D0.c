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
/* The 63 masks are 16-bit literals: cse shares one HImode constant register (sl) among
   them, and reload takes each SImode use from sl afresh because find_equiv_reg rejects a
   copy made in another mode. */
static inline u32 RandomValue(const u16 *table)
{
    u32 index = (gRandomIndex + 1) & 1023;
    gRandomIndex = index;
    return table[index];
}
static inline u32 RandomBits(const u16 *table, u16 mask)
{
    u32 index = (gRandomIndex + 1) & 1023;
    gRandomIndex = index;
    return table[index] & mask;
}
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

    if (ContactSequenceActor_TakeEntryFlag(actor)) {
        table = (const u16 *)RANDOM_TABLE_ADDR;
        scatter.bits.x = RandomValue(table) & 63;
        scatter.bits.y = RandomBits(table, 127);
        scatter.bits.z = RandomValue(table) & 63;
        scatterPosition = &scatter.position;
        scatterPosition->x += actor->position.x;
        scatterPosition->y += (position = &actor->position)->y;
        scatterPosition->z += position->z;
        TransientSpriteEffects_Spawn(0, 0x1004, scatterPosition, 0, 256, 256);
        {
            u32 sx = (u16)(RandomValue(table) & 63), sy, sz;
            scatterPosition->x = sx;
            sy = RandomBits(table, 127);
            scatterPosition->y = sy;
            sz = (u16)(RandomValue(table) & 63);
            sx += (u16)actor->position.x;
            scatterPosition->x = sx;
            sy += (u16)position->y;
            scatterPosition->y = sy;
            scatterPosition->z = (u16)position->z + sz;
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
