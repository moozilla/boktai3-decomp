#include "global.h"
#include "ui/widgets/scene_input.h"
#include "ui/town/storage_screen.h"
void Storage_RefreshCategoryWidgets(struct StorageScreen *);
void Storage_RefreshSelectedItemDetails(struct StorageScreen *);
void Storage_SetCategoryPalette(struct StorageScreen *, u16 *);
void Storage_UpdateCategoryBrowse(struct StorageScreen *);
void Storage_SetState(struct StorageScreen *, void (*)(struct StorageScreen *), u32);
void Storage_BeginCategoryBrowse(struct StorageScreen *screen)
{
    AnimSprite_Hide(&screen->labels[2]);
    AnimSprite_Hide(&screen->controls[6]);
    Storage_RefreshCategoryWidgets(screen);
    Storage_RefreshSelectedItemDetails(screen);
    Storage_SetCategoryPalette(screen, screen->cursorPalette);
    Storage_SetState(screen, Storage_UpdateCategoryBrowse, 0);
}
