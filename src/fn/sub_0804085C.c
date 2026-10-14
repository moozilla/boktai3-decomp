// MATCHING_CONCESSION: register-pinning
#include "modifier_list.h"
#define ModifierMenu_DrawBackground sub_0804085C
void sub_0804083C(struct ModifierMenu *);
void sub_082164AC(u32, void *, u32);
void BgGfx_LoadTilemapSimple(u32, u32, u32, u32, u32);
void ModifierMenu_DrawBackground(struct ModifierMenu *menu)
{
    register s32 visible asm("r4");
    switch (menu->state) {
        case 1:
            visible = 1;
            break;
        case 2:
            visible = 0;
            sub_0804083C(menu);
            break;
        case 0:
        default:
            visible = 0;
            break;
    }
    sub_082164AC(2, menu->backgroundResource, visible);
    BgGfx_LoadTilemapSimple(2, 0, 0, 0, 0);
}
