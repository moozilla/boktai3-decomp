#include "global.h"
// MATCHING_CONCESSION: register-pinning

// confidence=low: inferred from segment state and collider updates.

void Collider_SetAttackProperties(void *, s32, s32, s32, s32, s32);
void Collider_SetPosition(void *, void *, s32);
void Collider_Refresh(void *);
extern u8 *gUnk02000580;

#define H(p, o) (*(u16 *)((u8 *)(p) + (o)))
#define W(p, o) (*(u32 *)((u8 *)(p) + (o)))

struct P {
    u8 pad0[0x9c];
    s16 f9c;
    u8 pad1[0x2d0 - 0x9e];
    u32 f2d0;
    u16 f2d4;
    u8 pad2[0x3d0 - 0x2d6];
    u8 *f3d0;
};

static inline u32 tsth(struct P *a, u32 m)
{
    return a->f2d4 & m;
}
static inline u32 tstw(u32 m, u32 *a)
{
    return *a & m;
}

static inline u8 isset(u32 m, u8 *a)
{
    if (*a & m)
        return TRUE;
    return FALSE;
}

static inline u8 chk(struct P *p)
{
    if (p != 0) {
        u32 m = 0x40;
        {
            u32 o = 0x2d4;
            asm("" : "+r"(o));
            if (*(u16 *)((u8 *)p + o) & m)
                return TRUE;
            {
                u32 m2 = 0x20000;
                u32 *q;
                o -= 4;
                q = (u32 *)((u8 *)p + o);
                if (tstw(m2, q))
                    return TRUE;
            }
        }
        {
            register u32 d asm("r2");
            s32 v;
            asm("" : "=r"(d));
            v = p->f9c;
            asm("" ::"r"(d));
            if (v > 0)
                return FALSE;
        }
        return TRUE;
    }
    return FALSE;
}

static inline void SetAttack(void *collider, u8 *p, s32 variant)
{
    u8 *q = p + 0xb4 + variant;
    u32 a = *(u16 *)q;
    u32 b;
    u32 c;
    u32 d;
    q += 8;
    b = *(u16 *)q;
    q += variant ? 0x12 : 0x10;
    c = *(u32 *)q;
    q -= variant ? 0xa : 8;
    d = *(u16 *)q;
    Collider_SetAttackProperties(collider, a, b, 0, c, d);
}

void TentacleActor_UpdateSegmentColliders(struct P *pp)
{
    u8 *p = (u8 *)pp;
    u8 *base = pp->f3d0;
    s32 i;
    s16 d[3];
    s16 *dp;
    u8 *a, *b, *c, *fl;
    s32 off;

    if (chk(pp))
        return;
    i = 0;
    if (i >= *(s16 *)(base + 0x8da))
        return;
    dp = d;
    b = base + 0x41c;
    a = base + 0x3f0;
    c = base + 0x470;
    off = 0;
    fl = base + 0x4d9;
    do {
        u8 *g;
        u16 *q;
        if (!isset(1, fl)) {
            Collider_SetPosition(c, a, 0);
            Collider_Refresh(c);
            g = *(u8 **)0x02000580;
            q = (u16 *)(g + 0x30);
            {
                register u32 gx asm("r1") = H(g, 0x30);
                dp[0] = gx - H(a, 0);
            }
            dp[1] = q[1] - H(a, 2);
            dp[2] = q[2] - H(a, 4);
            if (dp[0] * dp[0] + dp[2] * dp[2] <= 0xFFFF) {
                Collider_SetPosition(b, a, 0);
                Collider_Refresh(b);
                if (tsth((struct P *)p, 0x100)) {
                    {
                        u32 mask = 0x100;
                        H(a, 0x32) &= ~mask;
                    }
                    SetAttack(b, p, 2);
                } else {
                    {
                        struct AttackFlags {
                            u8 pad[6];
                            u16 flags;
                        };
                        struct AttackFlags *f = (void *)(base + off + 0x41c);
                        f->flags |= 0x100;
                    }
                    SetAttack(b, p, 0);
                }
            }
        }
        b += 0x128;
        a += 0x128;
        c += 0x128;
        off += 0x128;
        fl += 0x128;
        i++;
    } while (i < *(s16 *)(base + 0x8da));
}
