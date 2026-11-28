#include "global.h"
#include "player/player.h"
#include "system/game_state/game_state.h"

u8 *LinkPlayer_GetSessionPlayerData(struct Player *);
void LinkPlayer_InitOptionalPlayerState(struct Player *);
void LinkPlayer_LoadBaseStats(struct Player *);
void LinkPlayer_InitHealth(struct Player *);
void LinkPlayer_InitEnergy(struct Player *);
void Player_SetTetherMode(struct Player *, s32, s32);
void Player_InitializeInputHistory(struct Player *);

// confidence=low: resets a link player: session data, stats, tether and input state.
s32 sub_0823ADB4(struct Player *p)
{
    p->actionConfig = (const struct PlayerActionConfig *)LinkPlayer_GetSessionPlayerData(p);
    p->kind = 1;
    *(u32 *)&gGameState->counters628[0] = 0;
    gGameState->counters628[1] = 0;
    *(u16 *)&gGameState->unk610 = 0;
    gGameState->unk612 = 0;
    LinkPlayer_InitOptionalPlayerState(p);
    LinkPlayer_LoadBaseStats(p);
    LinkPlayer_InitHealth(p);
    LinkPlayer_InitEnergy(p);
    Player_SetTetherMode(p, 0, 0);
    Player_InitializeInputHistory(p);
    p->sceneMode = 0;
    p->unk536 = 0;
    p->active = 0;
    p->unk549 = p->slot;
    p->lastContactFlags = 0;
    p->lastContactRewardFlags = 0;
    return 0;
}
