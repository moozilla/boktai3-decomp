#include "global.h"
u32 sub_08168E38(void *);
void PauseMenu_BeginEquipmentSelection(void *);
u32 PauseMenu_TryBeginEquipmentSelection(void *p)
{
    if (sub_08168E38(p)) {
        PauseMenu_BeginEquipmentSelection(p);
        return 1;
    }
    return 0;
}
