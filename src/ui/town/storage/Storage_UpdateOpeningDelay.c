#include "global.h"
#include "ui/town/storage_screen.h"
void Storage_UpdateBrowse(void);
void Storage_SetUpdateCallback(struct StorageScreen *, void (*)(void));
void Storage_UpdateOpeningDelay(struct StorageScreen *screen)
{
    screen->timer++;
    if (screen->timer > 0x1f)
        Storage_SetUpdateCallback(screen, Storage_UpdateBrowse);
}
