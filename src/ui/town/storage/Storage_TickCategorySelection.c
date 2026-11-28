#include "global.h"
#include "engine/input.h"
#include "ui/town/storage_screen.h"
#include "gba/io_reg.h"

s32 Storage_TickCategorySelection(struct StorageScreen *screen)
{
    if (gPadState[1] & L_BUTTON) {
        u32 category = screen->category;
        if (category == 0)
            category = screen->categoryCount;
        screen->category = category - 1;
        return 1;
    }
    if (gPadState[1] & R_BUTTON) {
        screen->category++;
        if (screen->category >= screen->categoryCount)
            screen->category = 0;
        return 1;
    }
    return 0;
}
