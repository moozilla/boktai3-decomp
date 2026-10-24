// MATCHING_CONCESSION: register-pinning
#include "global.h"
#define FrostEmitter_SpawnParticle sub_081D2D78
struct FrostVec {
    u16 x, y, z, pad;
};
struct FrostSequence {
    const u16 *commands;
    u8 flags, count, weight, duration;
    u16 index, unknownA, speed, elapsed;
};
struct FrostParticle {
    u32 flags;
    u8 unknown4[3], priority;
    u8 unknown8[8];
    u16 frame;
    u8 unknown12[10];
    struct FrostVec position;
    u8 unknown24[8];
    struct FrostSequence sequence;
    struct FrostVec velocity;
    u16 active, unknown46;
};
struct FrostEmitter {
    u8 unknown0[0x18];
    u8 graphics[0x1C];
    void *resource;
    struct FrostVec *source;
    u32 emitting;
    struct FrostParticle particles[12];
    u32 enabled, age;
};
extern u32 gUnk_03005308;
extern const u16 gUnk_0203B400[];
extern const s16 gUnk_086149C4[];
void Anim_SetSequence(void *, void *, u16, u8, u8);
static inline s32 ShiftRandom(u16 value, s32 shift)
{
    return value >> shift;
}
static inline s32 Scale(s32 n)
{
    return n >= 0 ? n >> 12 : -(-n >> 12);
}

void FrostEmitter_SpawnParticle(struct FrostEmitter *input)
{
    register struct FrostEmitter *p asm("r6") = input;
    struct FrostParticle *slot = 0;
    s32 i;
    for (i = 0; i < 12; i++) {
        if (!p->particles[i].active) {
            slot = &p->particles[i];
            break;
        }
    }
    if (slot) {
        u32 observed;
        const u16 *table;
        u32 index;
        u32 secondIndex;
        s32 angle;
        s32 magnitude;
        s32 mask;
        s32 sample;
        struct FrostVec *velocity;
        slot->position = *p->source;
        /* Retain the unused hardware address from the inlined random generator. */
        observed = 0x0403B714;
        table = gUnk_0203B400;
        index = (gUnk_03005308 + 1) & 1023;
        angle = ShiftRandom(table[index], 4);
        mask = 63;
        secondIndex = (index + 1) & 1023;
        gUnk_03005308 = secondIndex;
        sample = ShiftRandom(table[secondIndex], 4) & 1;
        velocity = &slot->velocity;
        angle &= mask;
        magnitude = sample + 1;
        velocity->x = Scale(gUnk_086149C4[(angle + 64) & 255] * magnitude);
        velocity->y = 0;
        velocity->z = Scale(gUnk_086149C4[angle] * magnitude);
        gUnk_03005308 = (gUnk_03005308 + 1) & 1023;
        asm volatile("" : "+r"(observed));
        slot->velocity.y =
            (ShiftRandom(((const u16 *)RANDOM_TABLE_ADDR)[gUnk_03005308], 4) & 3) + 4;
        slot->active = 60;
        Anim_SetSequence(&slot->sequence, p->resource, 0, 0, 4);
        slot->flags &= ~1;
    }
}
