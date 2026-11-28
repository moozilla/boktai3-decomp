#include "global.h"
#include "player/player.h"
#include "engine/video/render.h"
#include "ui/pause/scene_hud_display.h"
void SceneHud_RefreshGameplayDisplay(struct SceneHudState *);
s32 Hud_RefreshPlayerBinding(struct SceneHudState *hud)
{
    if (!(gEnginePauseFlags & 1)) {
        u32 slot = hud->actionOwnerIndex;
        struct Player *player = gPlayers[slot];
        hud->actionOwner = player;
        SceneHud_RefreshGameplayDisplay(hud);
    }
    return 0;
}
