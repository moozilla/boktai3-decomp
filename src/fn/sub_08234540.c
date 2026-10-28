#include "global.h"

extern u32 gRandomIndex;
extern const u16 gUnk_0203B400[];
extern const s16 gUnk_086149C4[];
u32 AnimData_FindEntry(u32);
void ParticleSprite_Initialize(void *, u32, s32);
void Effect_SetOrigin(void *, s32, s32);
void Effect_SetPalette(void *, s32);
void SpriteDrawable_SelectTileFrame(void *, u32, s32);

struct SbE88 {
    u8 f0;
    u8 pad1[7];
    u16 x;
    u16 f1A;
    u16 y;
    u8 pad[10];
    u8 angle;
    u8 f19;
    u8 f1Aa;
    u8 f1B;
    u8 f1C;
    u8 f1D;
    u16 f1E;
    u8 pad20[0x30 - 0x20];
};

static inline s32 scale(s32 product)
{
    return product >= 0 ? product >> 12 : -((-product) >> 12);
}

static inline u32 randomValue(const u16 *table, u32 mask)
{
    u32 offset;
    gRandomIndex = (gRandomIndex + 1) & mask;
    offset = gRandomIndex * 2;
    return *(const u16 *)(offset + (u32)table);
}

// MATCHING_CONCESSION: register-pinning
void SpiralBurst_InitParticles(u8 *p)
{
    register u32 mask asm("sb");
    s32 i;
    u32 base;
    u8 *s;
    register const u16 *table asm("r5");
    register const u16 *randomTable asm("r6");
    struct SbE88 *e;
    {
        u32 next;
        table = gUnk_0203B400;
        next = gRandomIndex + 1;
        mask = 1023;
        gRandomIndex = next & mask;
        base = (u8)table[gRandomIndex];
    }
    *(u32 *)(p + 0x1F8) = AnimData_FindEntry(0x1C1E);
    i = 0;
    randomTable = table;
    e = (struct SbE88 *)(p + 0x88);
    s = p + 0x78;
    for (; i < 8; i++) {
        ParticleSprite_Initialize(s, *(u32 *)(p + 0x1F8), 0);
        Effect_SetOrigin(s, -4, -4);
        Effect_SetPalette(s, 1);
        e->angle = base + (i << 5) + (randomValue(randomTable, mask) & 15);
        e->f19 = ((i & 3) << 1) + 3;
        e->f1E = (randomValue(randomTable, mask) & 31) + 64;
        e->f1Aa = (randomValue(randomTable, mask) & 15) + 12;
        e->f1C = 7;
        e->x = *(u16 *)(p + 0x1C) + scale(e->f1E * gUnk_086149C4[(e->angle + 64) & 255]);
        e->f1A = *(u16 *)(p + 0x1E);
        {
            s32 radius = e->f1E;
            const s16 *sn = gUnk_086149C4;
            asm("" : "+r"(sn));
            e->y = *(u16 *)(p + 0x20) + scale(radius * sn[e->angle]);
        }
        s[0xF] = 2;
        e->f0 = 0xEC;
        {
            u32 v = randomValue(randomTable, mask) & 7;
            e->f1B = v;
            if (v <= 3)
                SpriteDrawable_SelectTileFrame(s, *(u32 *)(p + 0x1F8), 12);
            else
                SpriteDrawable_SelectTileFrame(s, *(u32 *)(p + 0x1F8), 13);
        }
        e->f1D = (randomValue(randomTable, mask) & 15) + 0x38;
        e++;
        s += 0x30;
    }
}
