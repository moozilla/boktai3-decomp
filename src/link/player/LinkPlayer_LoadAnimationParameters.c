#include "global.h"
#include "engine/video/render.h"
#include "link/player/link_player_palette.h"

void LinkPlayer_LoadAnimationParameters(struct LinkPlayerFade *p)
{
    const u16 *colors = (const u16 *)(gPaletteBankData + 53 * 32);

    colors += p->animIdx * 3;
    p->basePalette[5] = *colors++;
    p->basePalette[6] = *colors++;
    p->basePalette[13] = *colors;
}
