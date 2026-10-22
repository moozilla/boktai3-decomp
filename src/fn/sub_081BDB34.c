#include "global.h"
struct StorageWidget {
    u32 unknown0[2];
    u32 flags;
};
void Storage_SetInteractionMode(u8 *, u32);
void Storage_GetSlotPosition(void *, s32);

void SceneInventory_SetState(u8 *p, u32 state, u32 mode)
{
    *(u32 *)(p + 0x17F4) = state;
    Storage_SetInteractionMode(p, mode);
    *(u16 *)(p + 0x174E) = 0;
    if (*(u8 *)(p + 0x174C) == 0) {
        ((struct StorageWidget *)(p + 0x13F8))->flags &= ~1;
        switch (*(u32 *)(p + 0x1750)) {
            case 1:
                Storage_GetSlotPosition(p + 0x1418, 0x22);
                break;
            case 3:
                Storage_GetSlotPosition(p + 0x1418, 0x21);
                break;
            case 2:
                Storage_GetSlotPosition(p + 0x1418, 0x20);
                break;
        }
    } else {
        ((struct StorageWidget *)(p + 0x13F8))->flags |= 1;
    }
}
