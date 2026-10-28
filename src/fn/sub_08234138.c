// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "ram.h"

struct E {
    u32 flags;
    u8 pad4[0xB];
    u8 fF;
    u8 f10;
    u8 pad11[7];
    s16 x, y, z;
    u8 pad1E[0xA];
    u16 angle;
    u16 radius;
};
struct S {
    u8 pad0[0x1C];
    u32 flags;
    u8 pad20[0x44];
    s16 x, y, z;
    u8 pad6A[2];
    struct E e[8];
    u32 frames;
    u8 pad1D0[0xA];
    u16 timer;
};
extern const s16 gUnk_086149C4[];
extern u16 gUnk_0203B400[];
u32 AnimData_FindEntry(u32);
void ParticleSprite_Initialize(void *, u32, u32);
void Effect_SetOrigin(void *, s32, s32);
void Effect_SetPalette(void *, u32);
void SpriteDrawable_SelectTileFrame(void *, u32, u32);

static inline int scale(int n)
{
    return n >= 0 ? n >> 12 : -((-n) >> 12);
}
static inline u32 rand8(void)
{
    u32 offset;
    gRandomIndex = (gRandomIndex + 1) & 1023;
    offset = gRandomIndex * 2;
    return *(const u8 *)(offset + (u32)gUnk_0203B400);
}
static inline u32 rand16t(const u16 *t)
{
    u32 offset;
    gRandomIndex = (gRandomIndex + 1) & 1023;
    offset = gRandomIndex * 2;
    return *(const u16 *)(offset + (u32)t);
}

void sub_08234138(struct S *p)
{
    s32 i;
    u32 seed = rand8();
    p->frames = AnimData_FindEntry(0x1c1e);
    for (i = 0; i < 8; i++) {
        struct E *e = &p->e[i];
        ParticleSprite_Initialize(e, p->frames, 0);
        Effect_SetOrigin(e, -4, -4);
        SpriteDrawable_SelectTileFrame(e, p->frames, 7);
        Effect_SetPalette(e, 1);
        {
            register const u16 *t asm("r5") = gUnk_0203B400;
            p->e[i].angle = (seed + i * 32 + (rand16t(t) & 15)) & 255;
            p->e[i].radius = (rand16t(t) & 0x1ff) + 256;
        }
        e->x = p->x + scale(p->e[i].radius * gUnk_086149C4[(p->e[i].angle + 64) & 255]);
        e->y = 300 + p->y;
        e->z = p->z + scale(p->e[i].radius * gUnk_086149C4[(u8)p->e[i].angle]);
        e->fF = 2;
        e->f10 = 0xec;
    }
}
