// MATCHING_CONCESSION: register-pinning
#include "global.h"

extern const s16 gUnk_086149C4[];
u32 AnimData_FindEntry(u32);
void ParticleSprite_Initialize(void *, u32, s32);
void Effect_SetOrigin(void *, s32, s32);
void Effect_SetPalette(void *, s32);
void SpriteDrawable_SelectTileFrame(void *, u32, s32);
void Collider_Refresh(void *);
void Task_Kill(void *);

struct SbElem {
    u8 pad0[0xF];
    u8 fF;
    u8 f10;
    u8 pad11[7];
    u16 x;
    u16 f1A;
    u16 y;
    u8 pad1E[10];
    u8 angle;
    u8 f29;
    u8 f2A;
    u8 f2B;
    u8 f2C;
    u8 f2D;
    u16 f2E;
};

struct SbPart {
    u16 x;
    u16 f2;
    u16 y;
    u8 pad6[10];
    u8 angle;
    u8 f11;
    u8 f12;
    u8 f13;
    u8 f14;
    u8 f15;
    u16 f16;
    u8 pad18[0x18];
};

struct SbOwner {
    u8 pad0[0x1C];
    u16 px;
    u16 pz;
    u16 py;
    u8 pad22[0x24 - 0x22];
    u8 collider[0x78 - 0x24];
    struct SbElem el[8];
    u32 res;
    u8 pad1FC[0x202 - 0x1FC];
    u16 timer;
};

static inline s32 scale(s32 product)
{
    return product >= 0 ? product >> 12 : -((-product) >> 12);
}

// still differs: pool constant 0x202 is shared across the call and the loop-back branch layout
void SpiralBurst_UpdateParticles(struct SbOwner *p)
{
    u32 alive = 0;
    s32 i;
    register u8 *s;
    u32 off;
    struct SbPart *e;
    u16 *tp;
    {
        register u32 offset asm("r1") = 0x202;
        register u16 *address;
        address = (void *)((u8 *)p + offset);
        if (*address <= 11)
            Collider_Refresh(p->collider);
    }
    asm("" : "+r"(p)::"r4");
    tp = &p->timer;
    off = 0;
    s = (u8 *)p + 0x78;
    e = (struct SbPart *)((u8 *)p + 0x90);
    i = 7;
    do {
        if (*tp >= e->f15) {
            u8 *q = (u8 *)p + 0x78;
            *(u32 *)(q + off) |= 1;
        } else {
            alive = 1;
            e->angle += e->f14;
            e->x = p->px + scale(e->f16 * gUnk_086149C4[(e->angle + 64) & 255]);
            asm("" ::: "memory");
            e->y = p->py + scale(e->f16 * gUnk_086149C4[e->angle]);
            if (*tp >= e->f12) {
                if ((*tp & 7) == 0 && e->f14 > 2)
                    e->f14--;
                e->f2 += e->f11;
            }
            if (e->f13 == 0)
                SpriteDrawable_SelectTileFrame(s, p->res, 12);
            else if (e->f13 == 4) {
                u32 q = off + 0x78;
                SpriteDrawable_SelectTileFrame((u8 *)p + q, p->res, 13);
            }
            e->f13++;
            if (e->f13 > 7)
                e->f13 = 0;
        }
        off += 0x30;
        s += 0x30;
        e++;
    } while (--i >= 0);
    p->timer++;
    if (!alive)
        Task_Kill(p);
}
