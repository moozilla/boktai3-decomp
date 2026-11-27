#include "global.h"
#include "engine/engine.h"
#include "system/game_state/game_state.h"
#include "player/movement/player_saved_position.h"
#include "engine/ram.h"
#include "link/session/link_session_state.h"
extern struct AreaEffectLinkState *gLinkSession;
void LinkPlayer_ReadSlotFromScript(void *);
void sub_0823ADB4(struct Player *);
void KeyframeActor_InitBodyFromScript(struct Player *, void *);
void Player_InitBodyCollider(struct Player *);
void LinkPlayer_InitAbilityState(struct Player *);
void Player_InitConfiguredAction(struct Player *);
void Player_InitOptionalComponents(struct Player *);
void LinkPlayer_InitializeActor(struct Player *);
static inline s32 LocalPlayer(void)
{
    if (!gLinkSession)
        return -1;
    return gLinkSession->localPlayer;
}
s32 Player_OnCreate(struct Player *p, void *script)
{
    LinkPlayer_ReadSlotFromScript(p);
    sub_0823ADB4(p);
    KeyframeActor_InitBodyFromScript(p, script);
    Player_InitBodyCollider(p);
    LinkPlayer_InitAbilityState(p);
    Player_InitConfiguredAction(p);
    Player_InitOptionalComponents(p);
    LinkPlayer_InitializeActor(p);
    gPlayerSlotStatus[p->slot] = p->kind;
    if (p->slot == LocalPlayer()) {
        Player_SavePositionAndFacing(p);
    }
    gPlayers[p->movementRequestBlocked] = p;
    gPlayerCount++;
    return 0;
}
