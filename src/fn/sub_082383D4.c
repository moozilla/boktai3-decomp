#define Player_SetTetherMode sub_082383D4
#include "global.h"
#include "player.h"
void sub_082383D4(struct Player *p, u32 mode, u32 phase)
{
    p->tetherMode = mode;
    p->tetherPhase = phase;
    p->tetherFrame = 0;
}
