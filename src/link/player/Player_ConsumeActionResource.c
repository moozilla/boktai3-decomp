#include "global.h"
#include "player/player.h"
#include "link/session/link_session_state.h"

s32 LinkPlayer_GetSelectedAbilityCost(struct Player *);

void Player_ConsumeActionResource(struct Player *p)
{
    s32 cost = LinkPlayer_GetSelectedAbilityCost(p);

    if (p->currentStatB < cost) {
        if (gLinkSession)
            gLinkSession->energyConsumed += p->currentStatB;
        p->currentStatB = 0;
    } else {
        if (gLinkSession)
            gLinkSession->energyConsumed += cost;
        p->currentStatB -= cost;
    }
}
