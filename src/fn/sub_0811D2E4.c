#include "global.h"
#include "game_state.h"
struct P {
    u8 pad[0x48];
    u8 a, b, c, d, e;
};
void sub_0811D2E4(struct P *p)
{
    u8 *g = gGameState;
    p->a = g[0x7a4] + 12;
    p->e = *(u16 *)(g + 0x7fe);
    p->c = g[0x7a2] + 18;
    p->b = g[0x7a0] + 6;
}