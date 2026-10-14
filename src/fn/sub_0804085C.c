#include "modifier_list.h"
void ModifierMenu_ResetBackground(struct ModifierMenu *);
void sub_082164AC(u32, void *, u32);
void BgGfx_LoadTilemapSimple(u32, u32, u32, u32, u32);
void ModifierMenu_DrawBackground(struct ModifierMenu *menu)
{
    s32 visible;
    /* do-while(0) only keeps agbcc's block layout */
    do {
        switch (menu->state) {
            case 1:
                visible = 1;
                break;
            case 2:
                visible = 0;
                ModifierMenu_ResetBackground(menu);
                break;
            case 0:
            default:
                visible = 0;
                break;
        }
        sub_082164AC(2, menu->backgroundResource, visible);
        BgGfx_LoadTilemapSimple(2, 0, 0, 0, 0);
    } while (0);
}
