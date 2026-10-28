#include "global.h"
#include "player.h"
struct S {
    u8 f[0x42c];
    u16 x;
};
void Player_InitField42C(void)
{
    struct S *s = (*(struct S **)gPlayers);
    if (s)
        s->x = 1000;
}
