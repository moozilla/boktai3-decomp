#include "global.h"
#include "ui/pause/scene_hud_display.h"
#include "engine/ram.h"
#include "player/player.h"
#include "link/session/link_session_state.h"

void Hud_LoadLayoutResources(struct SceneHudState *);
void SceneHud_InitGameplayResources(struct SceneHudState *);
void SceneHud_RefreshGameplayDisplay(struct SceneHudState *);
static inline s32 OwnerIndex(void)
{
    if (!gLinkSession)
        return -1;
    return gLinkSession->localPlayer;
}
s32 SceneHud_InitializeOwner(struct SceneHudState *hud)
{
    struct Player *player;
    hud->initializationFlag = 0;
    Bg_SetPriority(0, 0);
    hud->actionOwnerIndex = OwnerIndex();
    hud->initializationState = 0;
    {
        u8 index = hud->actionOwnerIndex;
        player = gPlayers[index];
    }
    hud->actionOwner = player;
    Hud_LoadLayoutResources(hud);
    SceneHud_InitGameplayResources(hud);
    SceneHud_RefreshGameplayDisplay(hud);
    hud->previousCueGroup = 255;
    hud->previousCueSelection = -1;
    hud->previousPage = -1;
    gFieldHud = (u8 *)hud;
    return 0;
}
