#include "global.h"
#include "player.h"
#include "render.h"
struct FieldHud {
    u8 unknown0[0xA3F];
    u8 playerSlot;
    struct Player *player;
};
void SceneHud_RefreshGameplayDisplay(struct FieldHud *);
s32 Hud_RefreshPlayerBinding(struct FieldHud *hud)
{
    if (!(gEnginePauseFlags & 1)) {
        u32 slot = hud->playerSlot;
        struct Player *player = gPlayers[slot];
        hud->player = player;
        SceneHud_RefreshGameplayDisplay(hud);
    }
    return 0;
}
