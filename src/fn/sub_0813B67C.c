#define Player_CalculateMaximumStatA Player_GetMaxLife
#define Player_CalculateMaximumStatB Player_GetMaxEnergy
#include "global.h"
#include "player.h"
#include "game_state.h"
s32 Script_GetKeywordValue(u32, s32);
s32 Player_GetMaxLife(struct Player *);
s32 Player_GetMaxEnergy(struct Player *);
void Player_InitializeStatsFromScript(struct Player *p)
{
    p->maximumStatA = Player_GetMaxLife(p);
    if (Script_GetKeywordValue('l', 0))
        p->currentStatA = p->maximumStatA;
    else if (p->actionMode == 6)
        p->currentStatA = gGameState->alternateStatA;
    else
        p->currentStatA = gGameState->life;
    if (!p->currentStatA)
        p->currentStatA = 1;
    else if (p->currentStatA > p->maximumStatA)
        p->currentStatA = p->maximumStatA;
    p->maximumStatB = Player_GetMaxEnergy(p);
    if (Script_GetKeywordValue('e', 0))
        p->currentStatB = p->maximumStatB;
    else if (p->actionMode == 6)
        p->currentStatB = gGameState->alternateStatB;
    else
        p->currentStatB = gGameState->unk2c;
    if (p->currentStatB > p->maximumStatB)
        p->currentStatB = p->maximumStatB;
    if (Script_GetKeywordValue('t', 0))
        p->statHundredths = 1000;
    else
        p->statHundredths = gGameState->statHundredths;
    p->statusValue = 0;
    p->statScale = 64;
}
