#include "global.h"
#include "player.h"
struct S {
    u8 f[0x588];
    u16 h;
};
struct T {
    u8 f[0x522];
    u16 h;
};
void ControlPanel_UpdateSustainedInputTimer(struct S *p)
{
    if ((*(struct T **)gPlayers)->h > 3)
        p->h = p->h + 1;
    else
        p->h = 0;
}
