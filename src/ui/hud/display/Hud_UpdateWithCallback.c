#include "global.h"
#include "player/player.h"
#include "ui/pause/scene_hud_display.h"
s32 Callback_InvokeActor(void *, void *);
void InventoryPanel_UpdateCallbackGroups(struct SceneHudState *);
s32 Hud_UpdateWithCallback(struct SceneHudState *hud)
{
    struct Player *player = gPlayers[0];
    hud->actionOwner = player;
    if (hud->updateCallback)
        Callback_InvokeActor(hud, hud->updateCallback);
    InventoryPanel_UpdateCallbackGroups(hud);
    return 0;
}
