#include "global.h"
#include "link/player/link_player_palette.h"

void LinkPlayer_InitAnimationSlots(struct LinkPlayerFade *p)
{
    p->paletteBanks[0] = p->slot + 47;
    p->paletteBanks[1] = 366;
    p->paletteBanks[2] = 45;
    p->paletteBanks[3] = 368;
    p->paletteBanks[4] = 369;
    p->paletteBanks[5] = 44;
    p->paletteBanks[6] = 46;
    p->paletteBanks[7] = 43;
}
