#include "global.h"
#include "player.h"
#include "game_state.h"

s32 Player_GetMaxLife(struct Player *);
s32 Player_GetMaxEnergy(struct Player *);

void Player_ClampStatsAndSyncStatus(struct Player *p)
{
    s32 v;
    v = Player_GetMaxLife(p);
    p->maximumStatA = v;
    if (p->currentStatA > (u16)v)
        p->currentStatA = v;
    v = Player_GetMaxEnergy(p);
    p->maximumStatB = v;
    if (p->currentStatB > (u16)v)
        p->currentStatB = v;
    p->statWhole = Div(p->statHundredths, 100);
    gGameState->playerStatusValue = p->statusValue;
}
