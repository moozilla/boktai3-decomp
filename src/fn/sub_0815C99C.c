#include "player.h"

void PlayerCue_ReleaseControl(struct Player *p)
{
    u32 z;
    if (p->tetherMode != 6)
        p->tetherMode = 0;
    p->tetherPhase = 0;
    p->tetherFrame = 0;
    z = 0;
    if (p->kindTwoEntered != 0)
        p->kindTwoEntered = z;
    p->kind = 1;
    p->terrainClampBlocked = z;
}
