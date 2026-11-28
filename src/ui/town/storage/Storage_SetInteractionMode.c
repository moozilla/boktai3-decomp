#include "global.h"
#include "ui/town/storage_screen.h"

void Storage_SetInteractionMode(struct StorageScreen *screen, u8 interactionMode)
{
    screen->interactionMode = interactionMode;
}
