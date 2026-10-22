#include "global.h"
#include "engine.h"

// confidence=low: initializes scripted cooldown lengths and the health threshold.
#define Axe_InitAttackCooldowns sub_080FAFD0

#ifdef NONMATCHING
// fuzzy: 95.1%
// Remaining difference: scratch register for the sign-extended health in the final quarter calculation (r3 vs r2).

struct Q080FAFD0 {
    u8 filler[0xeb4];
    u16 mid;
    u16 pad;
    u16 b[4];
    u16 v[4];
};

struct S080FAFD0 {
    u8 filler[0x9c];
    s16 health;
    u8 filler2[0x3d0 - 0x9e];
    struct Q080FAFD0 *q;
};

static inline s32 Quarter(s16 x)
{
    if (x >= 0)
        return x >> 2;
    return -(-x >> 2);
}

void Axe_InitAttackCooldowns(struct S080FAFD0 *s)
{
    struct Q080FAFD0 *q = s->q;
    s32 i;
    s32 h;
    int zero;
    unsigned long long hw;
    s16 *hp;
    if (Script_SeekToKeyword(0x50) != 0) {
        for (i = 0; i <= 3; i++) {
            q->v[i] = Script_GetPc() ? Script_GetValue() : 0;
        }
    }
    hp = &s->health;
    zero = 0;
    for (i = 3; i >= 0; i--)
        q->b[i] = zero;
    h = *hp;
    hw = h;
    q->mid = hw - Quarter(h);
}
#else
INCLUDE_ASM("asm/nonmatching", sub_080FAFD0);
#endif
