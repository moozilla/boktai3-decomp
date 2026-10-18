#include "global.h"
u32 SceneHud_CanOpenSecondarySelection(void *);
void PauseMenu_BeginEquipmentSelection(void *);
u32 PauseMenu_TryBeginEquipmentSelection(void *p)
{
    if (SceneHud_CanOpenSecondarySelection(p)) {
        PauseMenu_BeginEquipmentSelection(p);
        return 1;
    }
    return 0;
}
