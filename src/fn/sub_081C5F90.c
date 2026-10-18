#include "global.h"

struct E {
    u32 flags;
    u8 pad[0x5c];
};
struct S {
    u8 pad[0x120];
    struct E e[6];
};
void ChallengeResult_SetState(struct S *, void (*)(void));
void sub_081C6094(void);

void TimeResult_BeginDigitReveal(struct S *p)
{
    void (*f)(void) = sub_081C6094;
    s32 m = ~1;
    struct E *e = p->e;
    s32 i;
    for (i = 5; i >= 0; i--) {
        e->flags &= m;
        e++;
    }
    ChallengeResult_SetState(p, f);
}
