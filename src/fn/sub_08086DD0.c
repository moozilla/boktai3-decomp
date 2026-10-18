#include "global.h"
#define Bat_UpdateAnimationRequest sub_08086DD0

void SceneActorAnimation_SelectDirectionalEntry(void *, u32);

struct Seq {
    u8 pad0[7];
    u8 b7;
    u16 h8;
    u8 padA[4];
    u16 hE;
};

struct Obj {
    u8 pad0[4];
    u8 *inner;
};

static inline u8 TakeFlag(u8 *flag)
{
    if (*flag) {
        *flag = 0;
        return TRUE;
    }
    return FALSE;
}

static inline void Clr(u8 *q)
{
    ((struct Seq *)q)->hE = 0;
}

void sub_08086DD0(u8 *p)
{
    struct Seq *s;

    if (TakeFlag(p + 0x2B2))
        SceneActorAnimation_SelectDirectionalEntry(p, 5);
    s = (struct Seq *)(*(u8 **)(p + 4) + 0x48);
    if (s->h8 == 7 && s->hE == (s->b7 != 0) && s->b7 != 1) {
        s->h8 = 2;
        Clr(*(u8 **)(p + 4) + 0x48);
    }
}
