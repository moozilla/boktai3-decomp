#include "global.h"
s32 Div(s32, s32);
void Hud_DrawDefaultHealthMeter(void *, s32, s32, s32);
void sub_0813D834(void *, s32, s32);
struct QCF2C {
    u8 f[0x424];
    u16 done;
    u16 total;
};
struct SCF2C {
    u8 f[0xa40];
    struct QCF2C *q;
    u8 f2[0x4866 - 0xa44];
    s16 value;
};
void sub_0816CF2C(struct SCF2C *s, s32 n, s32 extra)
{
    struct QCF2C *q = s->q;
    s32 v;
    s32 sum;
    if (q == 0)
        return;
    v = Div(n * q->total, 100);
    s->value = v;
    sum = q->done + s->value;
    if (sum > q->total) {
        s->value = q->total - q->done;
        q->done = q->total;
    } else {
        q->done = sum;
    }
    Hud_DrawDefaultHealthMeter(s, 1, 2, 12);
    if (extra > 0)
        sub_0813D834(q, 0, extra * 60);
}
