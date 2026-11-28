#include "global.h"
#include "player/player.h"
#include "link/session/link_session_state.h"
s32 Script_GetOptionalIValue(void);
bool32 Script_SeekToKeyword(s32);
s32 Script_GetValue(void);
void LinkSession_CallPositionScript(struct SceneVec *, s32);
void Player_InitializeActionState(struct Player *, const struct PlayerActionConfig *);

void sub_0824372C(void)
{
    s32 i = Script_GetOptionalIValue();
    struct Player *player = gPlayers[i];
    if (player != NULL && Script_SeekToKeyword('w')) {
        s32 value = Script_GetValue();
        s32 slot;
        const struct PlayerActionConfig *config;
        if (player->unk549 >= 0)
            LinkSession_CallPositionScript(&player->position, player->unk549);
        player->unk549 = value;
        slot = player->unk549;
        if (gLinkSession == NULL || gLinkSession->unavailable || slot < 0)
            config = NULL;
        else
            config = (const struct PlayerActionConfig *)&gLinkSession->sessionData[4 + slot * 32];
        Player_InitializeActionState(player, config);
    }
}
