#define Hud_RefreshPlayerBinding sub_081788A8
#define gPlayerSlots gUnk_02000580
#include "global.h"
#include "player.h"
struct FieldHud { u8 unknown0[0xA3F]; u8 playerSlot; struct Player *player; };
extern u32 gUnk_0300523C;
void sub_08178820(struct FieldHud *);
s32 sub_081788A8(struct FieldHud *hud)
{
    if (!(gUnk_0300523C & 1)) {
        u32 slot = hud->playerSlot;
        struct Player *player = gUnk_02000580[slot];
        hud->player = player;
        sub_08178820(hud);
    }
    return 0;
}
