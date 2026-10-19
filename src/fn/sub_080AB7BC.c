#include "global.h"

#ifdef NONMATCHING
// fuzzy: 85.5%
// foundry draft (worker best 84.3%); what differs is mostly register allocation and instruction order
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

static inline u32 tsth(u16 *a, u32 m) { return *a & m; }
static inline u32 tstw(u32 m, u32 *a) { return *a & m; }

static inline u8 isset(u32 m, u8 *a) { if (*a & m) return TRUE; return FALSE; }

static inline u8 chk(struct P *p)
{
    if (p != 0) {
        u32 m = 0x40;
        u32 o = 0x2d4;
        if (*(u16 *)((u8 *)p + o) & m) return TRUE;
        o -= 4;
        if (tstw(0x20000, (u32 *)((u8 *)p + o))) return TRUE;
        if (p->f9c > 0) return FALSE;
        return TRUE;
    }
    return FALSE;
}

void sub_080AB7BC(struct P *pp)
{
    u8 *p = (u8 *)pp;
    u8 *base = pp->f3d0;
    s32 i;
    s16 d[3];
    u8 *a, *b, *c, *fl;
    s32 off;

    if (chk(pp)) return;
    i = 0;
    if (i >= *(s16 *)(base + 0x8da)) return;
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
            d[0] = H(g, 0x30) - H(a, 0);
            d[1] = q[1] - H(a, 2);
            d[2] = q[2] - H(a, 4);
            if (d[0] * d[0] + d[2] * d[2] <= 0xFFFF) {
                Collider_SetPosition(b, a, 0);
                Collider_Refresh(b);
                if (H(p, 0x2d4) & 0x100) {
                    H(a, 0x32) &= ~0x100;
                    Collider_SetAttackProperties(b, H(p, 0xb6), H(p, 0xbe), 0, W(p, 0xd0), H(p, 0x12));
                } else {
                    H(base + off + 0x41c, 6) |= 0x100;
                    Collider_SetAttackProperties(b, H(p, 0xb4), H(p, 0xbc), 0, W(p, 0xcc), H(p, 0x1));
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
#else
INCLUDE_ASM("asm/nonmatching", sub_080AB7BC);
#endif
