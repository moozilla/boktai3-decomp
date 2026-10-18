#define PlayerPanel_ClearNoticeTiles sub_08167864
#define MenuTilemap_GetCell Hud_GetTilemapCell
#include "global.h"
struct PlayerPanel {
    u8 unknown0[0xa9a];
    u8 noticeVisible;
};
extern u16 *MenuTilemap_GetCell(u32, s32, s32);
void sub_08167864(struct PlayerPanel *panel)
{
    if (panel->noticeVisible) {
        u16 *cell = MenuTilemap_GetCell(0, 0, 0);
        s32 i;
        for (i = 0; i < 9; i++)
            *cell++ = 0xd000;
        panel->noticeVisible = 0;
    }
}
