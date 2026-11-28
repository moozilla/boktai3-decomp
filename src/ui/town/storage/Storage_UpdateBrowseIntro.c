#include "global.h"
#include "ui/town/storage_screen.h"
void Storage_BeginCategoryBrowse(struct StorageScreen *);
void Storage_UpdateBrowseIntro(struct StorageScreen *screen)
{
    screen->timer++;
    if (screen->timer <= 11) {
        if ((screen->timer >> 1) & 1)
            screen->indicators[0].position.y = 104;
        else
            screen->indicators[0].position.y = 104 - ((20 - screen->timer) >> 3);
    } else if (screen->timer <= 15) {
        screen->indicators[0].position.y = 104;
    } else {
        Storage_BeginCategoryBrowse(screen);
    }
}
