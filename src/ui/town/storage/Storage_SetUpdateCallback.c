#include "global.h"
#include "ui/town/storage_screen.h"
void Storage_SetUpdateCallback(struct StorageScreen *screen, u32 callback)
{
    screen->updateCallback = callback;
    screen->timer = 0;
}
