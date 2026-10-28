#include "global.h"
#include "player.h"
#include "input.h"
struct PlayerDirectionHistory {
    u16 samples[10];
};
void Player_InitializeInputHistory(struct Player *s)
{
    s32 i;
    s->inputKeys = ((u16(*)[4])gPadState)[s->slot];
    s->catalogSavedOption = s->actionConfig->catalogOption;
    for (i = 0; i < 10; i++)
        ((struct PlayerDirectionHistory *)s->directionHistory)->samples[i] |= 0xFFFF;
}
