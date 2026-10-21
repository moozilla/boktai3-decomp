// MATCHING_CONCESSION: register-pinning
#include "global.h"
// differs only in how the 0x69c mask-offset constant is materialised (r8 setup)

extern void (*gUnk_08610808[])(void *, void *);

static inline u8 Tst(u32 *m, s32 i)
{
    if (*m & (1 << i))
        return TRUE;
    return FALSE;
}

struct E {
    u8 a[0x32];
    u8 kind;
    u8 b[0xD];
};
struct S {
    u8 pad[0x1C];
    struct E e[26];
    u8 pad2[0x69C - 0x1C - 0x40 * 26];
    u32 mask;
};

u32 ParticleEffectPool_Update(struct S *b)
{
    s32 i;
    u8 *k;
    struct E *e0 = b->e;
    struct E *e;
    register u32 off asm("r2") = 0x69C;
    register u32 *chk asm("r0");
    asm("" : "+r"(off));
    chk = (u32 *)((u8 *)b + off);
    if (*chk != 0) {
        u32 *fl;
        void (**tbl)(void *, void *);
        i = 0;
        fl = (u32 *)(0x69C + (u32)b);
        tbl = gUnk_08610808;
        k = &b->e[0].kind;
        e = e0;
        for (; i < 26; k += 0x40, e++, i++) {
            if (Tst(fl, i))
                tbl[*k](b, e);
        }
    }
    return 0;
}
