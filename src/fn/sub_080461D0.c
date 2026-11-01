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
// fuzzy: 97.5%
// Remaining difference: the ROM reloads xzMask (sl) afresh into r2 for the first `&`
// (movs r3, #0x3f; mov sl, r3; mov r2, sl), where this draft inherits r3. The same
// no-inheritance pattern appears in 91 ROM places; in matched DirectionalParticle_Spawn and
// ActorParticles_SpawnWideBurst it comes from a shared constant whose set is HImode (an `&`
// narrowed by a 16-bit store) and whose use is SImode, so find_equiv_reg rejects the mode.
// Writing every `& 63` as a narrowed literal shares one HImode pseudo, but it gets no register
// here (0x1004 takes sl): 8 differing insns.
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

    u32 xzMask;
    if (ContactSequenceActor_TakeEntryFlag(actor)) {
        table = (const u16 *)RANDOM_TABLE_ADDR;
        {
            u32 sample = RandomValue(table);
            xzMask = 63;
            scatter.bits.x = sample & xzMask;
        }
        scatter.bits.y = RandomBits(table, 127);
        scatter.bits.z = RandomBits(table, xzMask);
        scatterPosition = &scatter.position;
        scatterPosition->x += actor->position.x;
        scatterPosition->y += (position = &actor->position)->y;
        scatterPosition->z += position->z;
        TransientSpriteEffects_Spawn(0, 0x1004, scatterPosition, 0, 256, 256);
        {
            u32 sx = RandomBits(table, xzMask), sy, sz;
            scatterPosition->x = sx;
            sy = RandomBits(table, 127);
            scatterPosition->y = sy;
            sz = RandomBits(table, xzMask);
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

#else
INCLUDE_ASM("asm/nonmatching", ContactSequenceActor_UpdateDying);
#endif
