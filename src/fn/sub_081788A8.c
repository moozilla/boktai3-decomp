#define gPlayerSlots gUnk_02000580
#include "global.h"
#include "player.h"
struct FieldHud {
    u8 unknown0[0xA3F];
    u8 playerSlot;
    struct Player *player;
};
extern u32 gUnk_0300523C;
void SceneHud_RefreshGameplayDisplay(struct FieldHud *);
s32 Hud_RefreshPlayerBinding(struct FieldHud *hud)
{
    if (!(gUnk_0300523C & 1)) {
        u32 slot = hud->playerSlot;
        struct Player *player = gUnk_02000580[slot];
        hud->player = player;
        SceneHud_RefreshGameplayDisplay(hud);
    }
    return 0;
}
