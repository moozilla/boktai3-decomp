// MATCHING_CONCESSION: register-pinning
#include "global.h"
#ifdef NONMATCHING
// fuzzy: 90.2%
// fix.py best (sweep after the inlined-insn fix)
// fix.py best version (reprinted C)
// Differs: argument evaluation order of the final SpriteEffectController_Create call
// (0x1C1E/0xD43F pool loads come last in the original) and a few register choices.

struct Camera {
    u8 pad[0x44];
    s32 x;
};
extern struct Camera *gUnk_020004B8;
extern u8 *gUnk_030042E4;
extern const u16 gUnk_0203B400[];
extern u32 gUnk_03005308;
extern const s16 gUnk_086149C4[];
void Actor_UpdateVisibilityAndShadow(u32, u8 *);
void Palette_InterpolateBanks(void *, void *, void *, s32, s32);
struct ContactPosition {
    s16 x, y, z, pad;
} __attribute__((aligned(4)));
void *SpriteEffectController_Create(const struct ContactPosition *, const struct ContactPosition *,
                                    s32, u32, u32, u32, u32, u32, u32, u32, u32, u32, s32, s32, s32,
                                    s32, s32, s32);
struct Pair {
    u32 a, b;
};

static inline s32 Load(u32 addr)
{
    return *(const u16 *)addr;
}

static inline s32 Rand(void)
{
    gUnk_03005308 = (gUnk_03005308 + 1) & 0x3FF;
    return *(const u16 *)(gUnk_03005308 * 2 + (u32)gUnk_0203B400);
}

static inline s32 Truncate(s32 v)
{
    if (v >= 0)
        return v >> 12;
    return -((-v) >> 12);
}

s32 SceneNpc_OnStateEffectUpdate(u32 a_, u8 *p_)
{
    register u32 a asm("r12") = a_;
    u8 k2;
    u8 *p = p_;
    register u8 *q asm("r6") = p + 0x20;
    if (q[0xd] != 0) {
        register s32 zero asm("r8") = 0;
        s32 *pos;
        struct ContactPosition *dst;
        s32 adj;
        q[0xc] = 1;
        pos = (s32 *)(p + 0x300);
        dst = *((struct ContactPosition **)(p + 0x160));
        {
            register u32 off asm("r4") = 0x328;
            register u32 addr asm("r0");
            asm("" : "+r"(off));
            addr = ((u32)p) + off;
            asm("" : "+r"(addr));
            adj = *((s32 *)addr);
        }
        dst->x = (pos[0] >> 12) - gUnk_020004B8->x;
        dst->y = ((pos[2] + (pos[1] + adj)) >> 12) + 0x60;
        dst->z = pos[2] >> 12;
        {
            u16 *w = (u16 *)(p + 0x90);
            register u16 c asm("r0");
            *w = 4;
            w++;
            c = 0xFFE8;
            asm("" : "+r"(c));
            *w = c;
        }
        *((struct Pair *)(p + 0x68)) = *(*((struct Pair **)(p + 0x160)));
        *((u16 *)(p + 0x6c)) = zero;
        {
            u32 m = 0x200;
            asm("" : "+r"(m));
            *((u16 *)(p + 0xe)) = m | (*((u16 *)(p + 0xe)));
        }
    }
    Actor_UpdateVisibilityAndShadow(a, p);
    if ((q[0xc] == 0) && (p[6] <= 0x10)) {
        u32 m = 0x402;
        asm("" : "+r"(m));
        *((u16 *)(p + 0xe)) = m | (*((u16 *)(p + 0xe)));
    }
    {
        register u32 m asm("r1") = 4;
        asm("" : "+r"(m));
        if ((m & (*((u16 *)(p + 0xc)))) != 0) {
            s32 t;
            if ((*((s16 *)(q + 0xe))) <= 0x3f) {
                u8 *g = gUnk_030042E4;
                void *s1 = g + 0x2AE0;
                void *s2 = g + 0x2B00;
                void *d = p + 0x1CC;
                Palette_InterpolateBanks(d, s1, s2, *((s16 *)(q + 0xe)), 6);
            } else {
                u8 *g = gUnk_030042E4;
                void *s1 = g + 0x2AE0;
                void *s2 = g + 0x2B00;
                void *d = p + 0x1CC;
                Palette_InterpolateBanks(d, s1, s2, (k2 = 0x80) - (*((s16 *)(q + 0xe))), 6);
            }
            t = ((*((u16 *)(q + 0xe))) + 1) & 0x7f;
            *((u16 *)(q + 0xe)) = t;
            if (q[0xd] != 0) {
                register u32 mask asm("r12") = 3;
                if ((t & mask) == 0) {
                    struct ContactPosition pos_;
                    struct ContactPosition vel_;
                    struct ContactPosition *pos = &pos_;
                    s32 *pp = (s32 *)(p + 0x300);
                    s32 adj = *((s32 *)(p + 0x328));
                    pos->x = (pp[0] >> 12) - gUnk_020004B8->x;
                    pos->y = ((pp[2] + (pp[1] + adj)) >> 12) + 0x60;
                    pos->z = pp[2] >> 12;
                    {
                        register u32 opaque asm("r8") = 0x0403b714;
                        register s32 r1 asm("r1");
                        register s32 r2 asm("r0");
                        s32 e;
                        register u32 *cursor asm("r4");
                        asm volatile("" : "+r"(opaque));
                        {
                            register const u16 *table asm("r5") = gUnk_0203B400;
                            register u32 index asm("r0") =
                                ((cursor = &gUnk_03005308, (*cursor) + 1)) & 0x3FF;
                            r1 = (Load((index * 2) + ((u32)table)) >> 4) & 0x3F;
                            index = (index + 1) & 0x3FF;
                            *cursor = index;
                            r2 = (Load((index * 2) + ((u32)table)) >> 4) & mask;
                        }
                        {
                            register struct ContactPosition *vel asm("r3") = &vel_;
                            s32 ang = r1 + 0x60;
                            register s32 magp asm("r5") = r2 + 2;
                            register const s16 *sine asm("r2") = gUnk_086149C4;
                            s32 product = sine[(r1 + 0xA0) & 0xFF] * magp;
                            register u32 scratch asm("r6") = opaque;
                            register struct ContactPosition *v8 asm("r8");
                            asm volatile("" : : "r"(scratch));
                            v8 = vel;
                            vel->x = Truncate(product);
                            vel->y = Truncate(sine[ang] * magp);
                            {
                                register u32 z asm("r2") = 0;
                                vel->z = z;
                                *cursor = ((*cursor) + 1) & 0x3FF;
                                e = ((Load(((*cursor) * 2) + ((u32)gUnk_0203B400)) >> 4) & 0xF) +
                                    0xF;
                                SpriteEffectController_Create(&pos_, v8, 0x1C1E, 0xD43F, z, 0x10,
                                                              0x11, 1, 2, 1, z, e, z, z, z, z, -4,
                                                              -4);
                            }
                        }
                    }
                }
            }
        }
    }
    return 0;
}

#else
INCLUDE_ASM("asm/nonmatching", SceneNpc_OnStateEffectUpdate);
#endif
