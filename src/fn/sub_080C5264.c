#include "global.h"


struct Seq {
    u8 pad0[8];
    u16 h8;
    u8 padA[4];
    u16 hE;
};

struct Obj {
    u8 pad0[4];
    u8 *inner;
    u8 pad8[0x2D0 - 8];
    u32 flags;
};

extern u16 gUnk_08606120[];

u16 Skeleton_LookupAnimationEntry(struct Obj *p)
{
    struct Seq *s = (struct Seq *)(p->inner + 0x48);
    struct Seq *t = s;

    if (s->hE == 0) {
        u32 m = 0x10000;
        p->flags |= m;
        if (s->h8 <= 7)
            return gUnk_08606120[s->h8];
        if ((u16)(s->h8 - 8) <= 7)
            return gUnk_08606120[t->h8 - 8];
    }
    return 0;
}
