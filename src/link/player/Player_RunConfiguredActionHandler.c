#include "global.h"
#include "player/player.h"

void Player_SetTetherMode(struct Player *, u32, u32);
void Callback_InvokeActor(struct Player *, void *);

void Player_RunConfiguredActionHandler(struct Player *p)
{
    if (p->tetherMode != 3)
        Player_SetTetherMode(p, 3, 0);
    Callback_InvokeActor(p, p->actionCallback);
}
