#include "global.h"
#include "link/player/link_player_palette.h"

void LinkPlayer_InitAnimationState(struct LinkPlayerFade *p)
{
    LinkPlayer_InitAnimationSlots(p);
    p->modeArg = -1;
    p->animIdx = 0xFF;
    p->paletteBank = p->paletteBanks[0];
    p->palette = p->basePalette;
    p->target->paletteBank = p->paletteBanks[0];
    p->target->palette = p->basePalette;
    sub_08239A20(p, 0);
}
