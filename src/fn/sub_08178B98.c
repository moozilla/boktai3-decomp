#include "global.h"
#include "player.h"
struct FieldHud {
    u8 unknown0[0xA40];
    struct Player *player;
    u8 unknownA44[12];
    void *updateCallback;
};
s32 Callback_InvokeActor(void *, void *);
void InventoryPanel_UpdateCallbackGroups(struct FieldHud *);
s32 Hud_UpdateWithCallback(struct FieldHud *hud)
{
    struct Player *player = gPlayers[0];
    void *callback;
    hud->player = player;
    callback = hud->updateCallback;
    if (callback)
        Callback_InvokeActor(hud, callback);
    InventoryPanel_UpdateCallbackGroups(hud);
    return 0;
}
