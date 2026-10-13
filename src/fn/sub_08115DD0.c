#include "global.h"
struct S {
    u8 f[0x1c];
    u8 g[4];
    u8 on;
    u8 p[0x8f];
    u8 b0;
    u8 b1;
};
void Sprite_Remove(void *);
void sub_08115DD0(struct S *s)
{
    if (s->b1 != 0) {
        if (s->on != 0)
            Sprite_Remove(s->g);
        s->b0 = 0;
        s->b1 = 0;
    }
}
