#include "global.h"
struct QD308 { u8 f[0x418]; u8 mode; };
struct SD308 { u8 f0[0xa40]; struct QD308 *q; u8 f1[0x42a0 - 0xa44]; u8 state2; u8 f2[0x4870 - 0x42a1]; u8 state; u8 x; u16 y; };
void sub_0816D308(struct SD308 *s, u32 x, u32 y) {
    struct QD308 *q = s->q;
    if (q == 0) return;
    s->x = x;
    if (q->mode == 6) {
        if ((u8)x == 0) s->x = 2;
        else if ((u8)x == 2) s->x = 0;
    }
    s->y = y;
    s->state = 1;
    s->state2 = 1;
}
