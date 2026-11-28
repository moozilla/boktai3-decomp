#include "global.h"
#include "ui/town/storage_screen.h"
void Storage_SetInteractionMode(struct StorageScreen *, u32);
void Storage_GetSlotPosition(void *, s32);

void Storage_SetState(struct StorageScreen *screen, u32 state, u32 interactionMode)
{
    screen->state = state;
    Storage_SetInteractionMode(screen, interactionMode);
    screen->timer = 0;
    if (screen->interactionMode == 0) {
        AnimSprite_Show(&screen->labels[0]);
        switch (screen->mode) {
            case 1:
                Storage_GetSlotPosition(&screen->labels[0].position, 0x22);
                break;
            case 3:
                Storage_GetSlotPosition(&screen->labels[0].position, 0x21);
                break;
            case 2:
                Storage_GetSlotPosition(&screen->labels[0].position, 0x20);
                break;
        }
    } else {
        AnimSprite_Hide(&screen->labels[0]);
    }
}
