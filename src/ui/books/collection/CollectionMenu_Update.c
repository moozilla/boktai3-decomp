#include "global.h"
#include "engine/input.h"
#include "gba/io_reg.h"
#include "ui/books/collection.h"

void Sound_PlaySong(u32);
u32 CollectionMenu_IsValidGridPosition(s32, s32, s32);

/* Tentative: L/R change page, d-pad moves the cursor, B closes the menu.
 * Declared s32 with no return statement: the original leaves r0 live. */
s32 CollectionMenu_Update(struct CollectionMenu *menu)
{
    u32 pageChanged = 0;
    u32 moved = 0;

    if (menu->exiting == 0) {
        if (gPadState[1] & L_BUTTON) {
            if (--menu->page < 0)
                menu->page = 2;
            menu->column = 0;
            menu->row = 0;
            Sound_PlaySong(0x10a);
            pageChanged = 1;
        } else if (gPadState[1] & R_BUTTON) {
            if (++menu->page > 2)
                menu->page = 0;
            menu->column = 0;
            menu->row = 0;
            Sound_PlaySong(0x10a);
            pageChanged = 1;
        }
        if ((gPadState[1] & DPAD_UP) &&
            CollectionMenu_IsValidGridPosition(menu->column, menu->row - 1, menu->page)) {
            menu->row--;
            moved = 1;
            Sound_PlaySong(0xdc);
        } else if ((gPadState[1] & DPAD_DOWN) &&
                   CollectionMenu_IsValidGridPosition(menu->column, menu->row + 1, menu->page)) {
            menu->row++;
            moved = 1;
            Sound_PlaySong(0xdc);
        } else if ((gPadState[1] & DPAD_LEFT) &&
                   CollectionMenu_IsValidGridPosition(menu->column - 1, menu->row, menu->page)) {
            menu->column--;
            moved = 1;
            Sound_PlaySong(0xdc);
        } else if ((gPadState[1] & DPAD_RIGHT) &&
                   CollectionMenu_IsValidGridPosition(menu->column + 1, menu->row, menu->page)) {
            menu->column++;
            moved = 1;
            Sound_PlaySong(0xdc);
        }
        if (gPadState[1] & B_BUTTON) {
            ScreenTransition_SendCommand(1, menu->fadeShift, 4, 4, 4, 0xffff, 0);
            menu->exiting = 1;
            Sound_PlaySong(0x119);
        }
    }
    if (pageChanged || moved)
        menu->selectedEntry = menu->page * 10 + menu->row * 5 + menu->column;
    if (pageChanged)
        Collection_RefreshPage(menu);
    if (moved)
        Collection_RefreshSelection(menu);
    if (menu->exiting) {
        if (menu->fadeTimer > 1 << menu->fadeShift) {
            if (menu->exitScript)
                Script_Call(menu->exitScript, 0);
            Task_Kill(menu);
        }
        menu->fadeTimer++;
    } else {
        Collection_UpdateHighlightPulse(menu);
    }
}
