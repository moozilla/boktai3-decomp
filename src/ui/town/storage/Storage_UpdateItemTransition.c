#include "global.h"
#include "ui/town/storage_screen.h"
void sub_081BECF0(struct StorageScreen *);
void Storage_BeginCategoryBrowse(struct StorageScreen *);
void Storage_UpdateItemTransition(struct StorageScreen *screen)
{
    if (SceneInput_TickPairTransition(&screen->markers)) {
        sub_081BECF0(screen);
        Storage_BeginCategoryBrowse(screen);
    }
}
