#include "global.h"
struct S {
    u8 f[0xa8];
    u16 h;
    u8 b;
};
struct T {
    u32 fl;
    u8 p[8];
    u16 h;
};
void Collectible_ApplyPickupReward(void *, void *);
void Collectible_OnContact(struct S *s, struct T *t)
{
    u32 m = 0x20;
    if (t->fl & m) {
        s->b = 0;
    } else {
        Collectible_ApplyPickupReward(s, t);
        s->h = t->h;
    }
}
