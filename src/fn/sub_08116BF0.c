#include "global.h"
struct Q {
    u8 f[0x5c];
    u8 a[4];
    u8 b[0x14];
    s32 v;
};
struct G {
    u8 f[0x758];
    s16 h;
};
extern struct Q *gUnk_020004F0;
extern struct G *gGameState;
void Character_SetLowResourceColorEffect(void *, void *);
void Player_RestoreCurrentHealth(void)
{
    struct Q *q = gUnk_020004F0;
    if (q != 0) {
        q->v = gGameState->h;
        Character_SetLowResourceColorEffect(q->a, q->b);
    }
}
