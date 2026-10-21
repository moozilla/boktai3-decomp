// MATCHING_CONCESSION: register-pinning
#include "global.h"

// Remaining differences: allocation of the saturated defeat counter at offset 0x538.
extern u8 *gUnk_02000710;
void Sword_UpdateBodyPaletteRestore(void *);
void ChildGroup_ShowDamageNumber(void *, u32, u32);
void sub_080F421C(void);
#define H(p, n) (*(s16 *)((p) + (n)))
#define U(p, n) (*(u16 *)((p) + (n)))
#define W(p, n) (*(u32 *)((p) + (n)))
static inline s32 clamp(s32 v)
{
    if (v > 9999)
        v = 9999;
    else if (v < 0)
        v = 0;
    return v;
}
#define INC(n) H(gUnk_02000710, n) = clamp(H(gUnk_02000710, n) + 1)
struct SwordState {
    u8 pad0[0x8d];
    u8 flags;
    u8 pad8e[4];
    u8 entered, state;
};
static inline void setState(struct SwordState *p, u8 one, u8 id)
{
    p->entered = one;
    p->state = id;
    p->flags |= one;
}
static inline s32 SubtractDamage(u16 *health, u16 *damage)
{
    return *health - *damage;
}
void Sword_UpdateDamage(u8 *p)
{
    u32 flags;
    register u32 zero asm("r8");
    if (U(p, 0xcb4))
        U(p, 0xcb4)--;
    Sword_UpdateBodyPaletteRestore(p);
    if (U(p, 0xd7a)) {
        {
            s32 health = SubtractDamage(&U(p, 0x7e), &U(p, 0xd78));
            zero = 0;
            asm("" : "+r"(zero));
            H(p, 0x7e) = health;
        }
        if (H(p, 0x7e) < 0) {
            H(p, 0xd78) += H(p, 0x7e);
            H(p, 0x7e) = zero;
        }
        ChildGroup_ShowDamageNumber(p, (u16)H(p, 0xd7a), 0);
        H(p, 0xd7a) = zero;
        H(p, 0xd78) = zero;
        if (H(p, 0x7e) <= 0) {
            setState((void *)p, 1, 6);
            *(void (**)(void))(p + 0xcc8) = sub_080F421C;
            H(gUnk_02000710, 0x87C) =
                H(gUnk_02000710, 0x87C) < 9999 ? H(gUnk_02000710, 0x87C) + 1 : 9999;
            {
                register u8 *g asm("r0") = gUnk_02000710;
                register u32 off asm("r2") = 0x538;
                register s16 *q asm("r3");
                register u32 v asm("r4");
                q = (s16 *)(g + off);
                v = *(u16 *)q;
                *q = *q < 9999 ? v + 1 : 9999;
                asm("" ::"r"(v));
            }
            {
                register u32 read asm("r2") = W(p, 0xCA8);
                asm("" : "+r"(read));
                flags = read;
            }
            H(gUnk_02000710, 0x548) =
                H(gUnk_02000710, 0x548) < 9999 ? H(gUnk_02000710, 0x548) + 1 : 9999;
            if (flags & 0x80) {
                INC(0x58A);
            } else if (flags & 0x100) {
                INC(0x58C);
            } else if (flags & 0x200) {
                INC(0x58E);
            } else if (flags & 0x400) {
                INC(0x590);
            } else {
                register u32 last asm("r2") = flags;
                last &= 0x800;
                asm("" : "+r"(last));
                if (last) {
                    INC(0x592);
                }
            }
        }
    }
}
