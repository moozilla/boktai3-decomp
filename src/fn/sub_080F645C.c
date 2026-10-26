#include "global.h"

extern u8 *gGameState;
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
#define INC(n) H(gGameState, n) = clamp(H(gGameState, n) + 1)
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
    u32 zero;
    if (U(p, 0xcb4))
        U(p, 0xcb4)--;
    Sword_UpdateBodyPaletteRestore(p);
    if (U(p, 0xd7a)) {
        {
            s32 health = SubtractDamage(&U(p, 0x7e), &U(p, 0xd78));
            zero = 0;
            H(p, 0x7e) = health;
        }
        if (H(p, 0x7e) < 0) {
            H(p, 0xd78) += H(p, 0x7e);
            H(p, 0x7e) = zero;
        }
        ChildGroup_ShowDamageNumber(p, (u16)H(p, 0xd7a), 0);
        *((s16 *)(p + 0xd7a)) = zero;
        *((s16 *)(p + 0xd78)) = zero;
        if ((*((s16 *)(p + 0x7e))) <= 0) {
            do {
                setState((void *)p, 1, 6);
                *(void (**)(void))(p + 0xcc8) = sub_080F421C;
                H(gGameState, 0x87C) =
                    H(gGameState, 0x87C) < 9999 ? H(gGameState, 0x87C) + 1 : 9999;
                {
                    u8 *g = gGameState;
                    u32 off = 0x538;
                    s16 *q;
                    u32 v;
                    q = (s16 *)(g + off);
                    v = *(u16 *)q;
                    *q = *q < 9999 ? v + 1 : 9999;
                }
                {
                    u32 read = *((u32 *)(p + 0xCA8));
                    flags = read;
                }
                do {
                    *((s16 *)(gGameState + 0x548)) = ((*((s16 *)(gGameState + 0x548))) < 9999)
                                                         ? ((*((s16 *)(gGameState + 0x548))) + 1)
                                                         : (9999);
                    if (flags & 0x80) {
                        *((s16 *)(gGameState + 0x58A)) =
                            clamp((*((s16 *)(gGameState + 0x58A))) + 1);
                    } else if (flags & 0x100) {
                        *((s16 *)(gGameState + 0x58C)) =
                            clamp((*((s16 *)(gGameState + 0x58C))) + 1);
                    } else if (flags & 0x200) {
                        *((s16 *)(gGameState + 0x58E)) =
                            clamp((*((s16 *)(gGameState + 0x58E))) + 1);
                    } else if (flags & 0x400) {
                        *((s16 *)(gGameState + 0x590)) =
                            clamp((*((s16 *)(gGameState + 0x590))) + 1);
                    } else {
                        u32 last = flags;
                        last &= 0x800;
                        if (last) {
                            *((s16 *)(gGameState + 0x592)) =
                                clamp((*((s16 *)(gGameState + 0x592))) + 1);
                        }
                    }
                } while (0);
            } while (0);
        }
    }
}
