#include "global.h"
#include "player/player.h"

void Callback_InvokeActor(struct Player *, void (*)(void));

void Player_InvokeStoredCallback(struct Player *p)
{
    Callback_InvokeActor(p, p->actionCallback);
}
