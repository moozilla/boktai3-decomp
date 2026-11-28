#include "global.h"
#include "player/player.h"
#include "link/session/link_session_state.h"

u8 *LinkPlayer_GetSessionPlayerData(struct Player *p)
{
    s32 slot = p->slot;
    u8 *session = (u8 *)gLinkSession;

    if (session == NULL || ((struct AreaEffectLinkState *)session)->unavailable || slot < 0)
        return NULL;
    return session + (slot * 68 + 0x1C0);
}
