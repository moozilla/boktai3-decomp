#include "global.h"
#include "player.h"
struct S {
    u8 f[0x500];
    u8 b[0x20];
    u8 pd[2];
    u16 h;
};
void ControlPanel_ClearPlayerInputHistory(void)
{
    s32 i;
    for (i = 0; i <= 0x1f; i++)
        (*(struct S **)gPlayers)->b[i] = 0;
    (*(struct S **)gPlayers)->h = 0;
}
