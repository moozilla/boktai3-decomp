#define gPlayerSlotInputs gUnk_03005260
#include "global.h"
#include "player.h"
extern u16 gUnk_03005260[][4];
struct PlayerDirectionHistory {
    u16 samples[10];
};
void Player_InitializeInputHistory(struct Player *s)
{
    s32 i;
    s->inputKeys = gUnk_03005260[s->slot];
    s->catalogSavedOption = s->actionConfig->catalogOption;
    for (i = 0; i < 10; i++)
        ((struct PlayerDirectionHistory *)s->directionHistory)->samples[i] |= 0xFFFF;
}
