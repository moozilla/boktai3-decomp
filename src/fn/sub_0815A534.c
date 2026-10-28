#include "global.h"
#include "player.h"
struct S {
    u8 f[0x1c];
    s32 a;
};
u32 PlayerCue_IsStateFour(void)
{
    u32 r;
    if (!(*(struct S **)gPlayers))
        goto z;
    if ((*(struct S **)gPlayers)->a == 4)
        goto o;
z:
    r = 0;
    goto e;
o:
    r = 1;
e:
    return r;
}
