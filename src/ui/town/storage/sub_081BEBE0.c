#include "global.h"
#include "engine/input.h"
#include "ui/town/storage_screen.h"
#include "gba/io_reg.h"
void Sound_PlaySong(u32);
void Storage_UpdateCursorPosition(void *, void *);
void Storage_ShowItemDescription(void *);
void Storage_UpdateSwapSelection(void *);
void Storage_UpdateCategoryConfirmation(void *);
void Storage_SetState(void *, void (*)(void *), u32);
void MenuPanel_ShowDescription(void *, s32, void *, s32);
void sub_081BEBE0(struct StorageScreen *screen)
{
    if (gPadState[1] & B_BUTTON) {
        Sound_PlaySong(0x19a);
        Storage_UpdateCursorPosition(&screen->gridKind, &screen->labels[1]);
        Storage_ShowItemDescription(screen);
        Storage_SetState(screen, Storage_UpdateSwapSelection, 0);
    } else if (gPadState[1] & A_BUTTON) {
        screen->labels[1].flags |= 1;
        screen->indicators[4].flags |= 1;
        Sound_PlaySong(0x107);
        MenuPanel_ShowDescription(screen, 2, ((struct StorageTextView *)screen)->itemText, -1);
        SceneInput_BeginChoice(&screen->controls[6], &screen->commonResource, &screen->choiceState,
                               0, 0);
        Storage_SetState(screen, Storage_UpdateCategoryConfirmation, 0);
    }
}
