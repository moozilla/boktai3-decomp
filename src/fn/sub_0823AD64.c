#include "global.h"
#include "player.h"
extern u16 gPadState[][4];
struct PlayerDirectionHistory {
    u16 samples[10];
};
void Player_InitializeInputHistory(struct Player *s)
{
    s32 i;
    s->inputKeys = gPadState[s->slot];
    s->catalogSavedOption = s->actionConfig->catalogOption;
    for (i = 0; i < 10; i++)
        ((struct PlayerDirectionHistory *)s->directionHistory)->samples[i] |= 0xFFFF;
}
