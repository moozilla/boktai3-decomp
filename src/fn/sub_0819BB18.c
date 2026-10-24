#include "global.h"

// confidence=low: emits sixteen motion entries around the actor's facing direction.
#define Hresvelgr_EmitFacingMotionBurst sub_0819BB18
struct BurstVector {
    s16 x, y, z, pad;
};
struct BurstActor {
    u8 unknown0[0x51];
    u8 direction;
    u8 unknown52[2];
    struct BurstVector position;
    u8 unknown5C[0x110 - 0x5C];
    struct BurstVector *target;
};
extern const s16 gUnk_086149C4[];
extern const u16 gUnk_0203B000[];
extern u32 gUnk_03005308;
void MotionEntry_CreateWithDelta(void *, void *, s32, s32);
static inline s32 scale(s32 n)
{
    return n >= 0 ? n >> 12 : -((-n) >> 12);
}
static inline void polar(struct BurstVector *v, s32 angle, s32 radius)
{
    v->x = scale(gUnk_086149C4[(angle + 64) & 255] * radius);
    v->y = 0;
    v->z = scale(gUnk_086149C4[angle & 255] * radius);
}
static inline void add(struct BurstVector *d, struct BurstVector *a, struct BurstVector *b)
{
    d->x = b->x + a->x;
    d->y = b->y + a->y;
    d->z = b->z + a->z;
}
static inline s32 randomValue(const u16 *table)
{
    u32 offset;
    table += 512;
    gUnk_03005308 = (gUnk_03005308 + 1) & 1023;
    offset = gUnk_03005308 * 2;
    return *(const u16 *)(offset + (u32)table);
}
#ifdef NONMATCHING
// fix.py best (sweep after the inlined-insn fix)
// Remaining differences: random-table address lifetimes and loop register allocation.
void Hresvelgr_EmitFacingMotionBurst(struct BurstActor *actor)
{
    struct BurstVector offset;
    struct BurstVector position;
    struct BurstVector delta;
    s32 i;
    polar(&offset, (actor->direction + 128) & 255, 768);
    {
        struct BurstVector *target = actor->target;
        add(&position, target, &offset);
    }
    for (i = 0; i < 16; i++) {
        const u16 *table = *((typeof(gUnk_0203B000) *)0x0203B000);
        s32 type;
        s32 vertical;
        s32 sample = randomValue(table);
        polar(&delta, ((actor->direction + ((sample >> 3) % 64)) - 32) & 255, 128);
        type = (randomValue(table) >> 3) % 4;
        vertical = 32 - ((randomValue(table) >> 3) % 64);
        do {
            MotionEntry_CreateWithDelta(&actor->position, &delta, type, vertical);
        } while (0);
    }
}

#else
INCLUDE_ASM("asm/nonmatching", sub_0819BB18);
#endif
