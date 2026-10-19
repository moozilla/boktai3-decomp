#include "global.h"
/* Golem_UpdateDeath_080D211C: Golem_UpdateDeath variant: entry 9, and resets the facing entry while +0x2B3 is set (provisional name) */
#define Golem_UpdateDeath_080D211C sub_080D211C
void SceneActorAnimation_SelectDirectionalEntry(void *, u32);
static inline u32 orr(u32 k, u16 *a)
{
    return k | *a;
}
struct S {
    u8 f0[0x2b0];
    u8 b0;
    u8 f1;
    u8 b2;
    u8 b3;
    u8 f2[0x20];
    u16 h4;
    u16 h6;
};
static inline u8 tk(u8 *a)
{
    if (*a) {
        *a = 0;
        return 1;
    }
    return 0;
}
void Golem_UpdateDeath_080D211C(struct S *p, s32 n)
{
    if (tk(&p->b2)) {
        s32 m;
        u16 *a;
        SceneActorAnimation_SelectDirectionalEntry(p, 9);
        a = &p->h4;
        m = -2;
        *a = m & *a;
    }
    if (n == 0x28) {
        u16 k = 0x10;
        p->h6 = k | p->h6;
    }
    if (p->b3) {
        SceneActorAnimation_SelectDirectionalEntry(p, 0);
        if (n > 0x3C)
            p->b0 = 1;
    }
}
