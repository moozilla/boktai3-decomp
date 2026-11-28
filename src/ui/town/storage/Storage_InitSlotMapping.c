#include "global.h"
#include "ui/town/storage_screen.h"
void Storage_InitSlotMapping(struct StorageScreen *screen, s32 base)
{
    s32 i;
    for (i = 0; i < 16; i++)
        screen->slots[i] = i;
    for (i = 16; i < 32; i++)
        screen->slots[i] = i + base;
}
