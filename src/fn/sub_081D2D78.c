#include "global.h"
#define FrostEmitter_SpawnParticle sub_081D2D78
#ifdef NONMATCHING
// fix.py best (sweep after the inlined-insn fix)
// Remaining: free-slot search, RNG scheduling and vector/register lifetimes.
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
static inline u32 Rand(void)
{
    gUnk_03005308 = (gUnk_03005308 + 1) & 1023;
    return gUnk_0203B400[gUnk_03005308] >> 4;
}
static inline s32 Scale(s32 n)
{
    return n >= 0 ? n >> 12 : -(-n >> 12);
}
static inline void Polar(struct FrostVec *p, s32 angle, s32 magnitude)
{
    p->x = Scale(gUnk_086149C4[(angle + 64) & 255] * magnitude);
    p->y = 0;
    p->z = Scale(gUnk_086149C4[angle & 255] * magnitude);
}
inline static u32 Rand_081D2D78(void)
{
    gUnk_03005308 = (gUnk_03005308 + 1) & 1023;
    {
        s8 k1 = 4;
        return gUnk_0203B400[gUnk_03005308] >> k1;
    }
}

void FrostEmitter_SpawnParticle(struct FrostEmitter *p)
{
    struct FrostParticle *slot = 0;
    s32 i;
    for (i = 0; i < 12; i++) {
        if (!p->particles[i].active) {
            slot = &p->particles[i];
            break;
        }
    }

    if (slot) {
        s32 angle;
        s32 magnitude;
        slot->position = *p->source;
        angle = Rand_081D2D78() & 63;
        do {
            magnitude = (Rand_081D2D78() & 1) + 1;
        } while (0);
        Polar(&slot->velocity, angle, magnitude);
        slot->velocity.y = (Rand_081D2D78() & 3) + 4;
        slot->active = 60;
        Anim_SetSequence(&slot->sequence, p->resource, 0, 0, 4);
        slot->flags &= ~1;
    }
}

#else
INCLUDE_ASM("asm/nonmatching", sub_081D2D78);
#endif
