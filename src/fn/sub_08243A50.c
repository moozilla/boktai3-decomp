#include "global.h"
#include "engine.h"
#include "game_state.h"
#include "player_saved_position.h"
#include "ram.h"
struct LinkRef {
    u8 unk0[0x18];
    s16 bit;
};
extern struct LinkRef *gLinkSession;
void LinkPlayer_ReadSlotFromScript(void);
void sub_0823ADB4(struct Player *);
void KeyframeActor_InitBodyFromScript(struct Player *, void *);
void Player_InitBodyCollider(struct Player *);
void LinkPlayer_InitAbilityState(struct Player *);
void Player_InitConfiguredAction(struct Player *);
void Player_InitOptionalComponents(struct Player *);
void sub_0823A3CC(struct Player *);
static inline s32 LocalPlayer(void)
{
    if (!gLinkSession)
        return -1;
    return gLinkSession->bit;
}
s32 Player_OnCreate(struct Player *p, void *script)
{
    LinkPlayer_ReadSlotFromScript();
    sub_0823ADB4(p);
    KeyframeActor_InitBodyFromScript(p, script);
    Player_InitBodyCollider(p);
    LinkPlayer_InitAbilityState(p);
    Player_InitConfiguredAction(p);
    Player_InitOptionalComponents(p);
    sub_0823A3CC(p);
    gPlayerSlotStatus[p->slot] = p->kind;
    if (p->slot == LocalPlayer()) {
        Player_SavePositionAndFacing(p);
    }
    gPlayers[p->movementRequestBlocked] = p;
    gPlayerCount++;
    return 0;
}
