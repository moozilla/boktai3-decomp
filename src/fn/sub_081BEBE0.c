#include "global.h"
#include "input.h"
void Sound_PlaySong(u32);
void Storage_UpdateCursorPosition(void *, void *);
void Storage_ShowItemDescription(void *);
void Storage_UpdateSwapSelection(void *);
void SceneInventory_UpdateCategoryConfirmation(void *);
void SceneInventory_SetState(void *, void (*)(void *), u32);
void MenuPanel_ShowDescription(void *, s32, void *, s32);
void SceneInput_BeginChoice(void *, void *, u8 *, u32, u32);
void sub_081BEBE0(u8 *p)
{
    u16 cancel = gPadState[1] & 2;
    if (cancel) {
        Sound_PlaySong(0x19a);
        Storage_UpdateCursorPosition(p + 0x1760, p + 0x1458);
        Storage_ShowItemDescription(p);
        SceneInventory_SetState(p, Storage_UpdateSwapSelection, 0);
    } else if (gPadState[1] & 1) {
        *(u32 *)(p + 0x1460) |= 1;
        *(u32 *)(p + 0x10a0) |= 1;
        Sound_PlaySong(0x107);
        MenuPanel_ShowDescription(p, 2, *(void **)(p + 0x1734), -1);
        SceneInput_BeginChoice(p + 0x258, p + 0x1518, p + 0x174d, 0, 0);
        SceneInventory_SetState(p, SceneInventory_UpdateCategoryConfirmation, 0);
    }
}
