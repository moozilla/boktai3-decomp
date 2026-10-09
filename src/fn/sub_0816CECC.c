#include "global.h"
s32 Div(s32, s32);
void sub_0816446C(void *, s32, s32, s32);
struct QCECC { u8 f[0x424]; u16 done; u16 total; };
struct SCECC { u8 f[0xa40]; struct QCECC *q; u8 f2[0x4866 - 0xa44]; s16 value; };
void sub_0816CECC(struct SCECC *s, s32 n) {
    struct QCECC *q = s->q;
    s32 v;
    s32 sum;
    if (q == 0) return;
    v = Div(n * q->total, 100);
    s->value = v;
    sum = q->done + s->value;
    if (sum > q->total) {
        s->value = q->total - q->done;
        q->done = q->total;
    } else {
        q->done = sum;
    }
    sub_0816446C(s, 1, 2, 12);
}
