#include "global.h"
#include "menu_parameters.h"
struct RegionEntry {
    s16 page;
    u8 pad[10];
};
struct RegionMenu {
    u8 pad0[0x1c];
    s32 page;
    s32 count;
    u8 pad24[0x280 - 0x24];
    struct RegionEntry entries[(0x5b4 - 0x280) / 12];
    u8 pad5b0[4];
    s32 skipped;
};
void Menu_EraseRect(s32, s32, s32, s32);
void Menu_DrawStringRef(s32, s32, u32, s32);
void Time_ShowRegionMenu(struct RegionMenu *menu)
{
    s32 first = 1;
    s32 row = 0;
    s32 i;
    menu->skipped = row;
    Menu_EraseRect(4, 7, 0x14, 10);
    for (i = 0; i < menu->count; i++) {
        if (menu->entries[i].page == menu->page) {
            if (first == 1)
                first = 0;
            Menu_DrawStringRef(4, row + 7, gRegionMenuParameters.nBlock, i);
            row += 2;
        }
        menu->skipped += first;
    }
}
