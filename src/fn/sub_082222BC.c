#include "global.h"

struct PT {
    u32 a, b;
};
struct PolarInterp {
    u8 pad0[4];
    s32 cur;
    s32 start;
    s32 end;
    s32 curDist;
    s32 startDist;
    s32 endDist;
    u16 f1c;
    u16 f1e;
    u8 pad20[8];
    struct PT target;
};

void Math_GetTargetAngleAndDistance(struct PolarInterp *, struct PT *, u8 *, u16 *);

void sub_082222BC(struct PolarInterp *p, struct PT *pt, s32 n)
{
    u8 a;
    u16 b;
    s32 startv;
    s32 diff;
    s32 absd;
    struct PT *t = &p->target;
    *t = *pt;
    Math_GetTargetAngleAndDistance(p, t, &a, &b);
    startv = a << 8;
    p->start = startv;
    p->startDist = b << 8;
    p->f1c = 0;
    p->f1e = n;
    diff = startv - p->cur;
    absd = diff;
    if (absd < 0)
        absd = -absd;
    if (absd > 0x8000) {
        if (diff < 0)
            diff += 0x10000;
        else
            diff += -0x10000;
        p->start = p->cur + diff;
    }
    p->end = p->cur;
    p->endDist = p->curDist;
}
