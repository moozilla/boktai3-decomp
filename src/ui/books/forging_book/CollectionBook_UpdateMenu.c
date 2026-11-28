#include "global.h"
#include "ui/books/collection_book.h"
#include "engine/input.h"
#include "gba/io_reg.h"
struct ScriptArg {
    u16 type;
    void *value;
};
void Sound_PlaySong(u32);
u32 ForgingBook_IsGridPositionValid(s32, s32, s32);
void CollectionBook_UpdateTrackedEntryMarker(struct CollectionBook *);
void ScreenTransition_SendCommand(s32, s32, s32, s32, s32, s32, s32);
void ForgingBook_RefreshPage(struct CollectionBook *);
void sub_0821353C(struct CollectionBook *);
void ForgingBook_UpdateCursorPulse(struct CollectionBook *);

// confidence=low: handles collection-grid selection and the menu close transition.
s32 CollectionBook_UpdateMenu(struct CollectionBook *book)
{
    u32 pageChanged = 0;
    u32 moved = 0;
    s32 values[2];
    struct ScriptArg arg;

    if (book->exiting == 0 && book->busy == 0) {
        if (gPadState[1] & B_BUTTON) {
            ScreenTransition_SendCommand(1, book->fadeShift, 4, 4, 4, 0xffff, 0);
            book->exiting = 1;
            book->closing = 1;
            Sound_PlaySong(0x119);
        } else if (gPadState[1] & A_BUTTON) {
            if (CollectionBook_IsEntryCollected(book->entryIds[book->selectedEntry]) &&
                book->trackingLocked == 0) {
                Sound_PlaySong(0xdd);
                book->trackedEntry = book->entryIds[book->selectedEntry];
                CollectionBook_UpdateTrackedEntryMarker(book);
            } else {
                Sound_PlaySong(0x192);
            }
        } else if ((gPadState[1] & DPAD_UP) &&
                   ForgingBook_IsGridPositionValid(book->column, book->row - 1, book->page)) {
            book->row--;
            moved = 1;
            Sound_PlaySong(0xdc);
        } else if ((gPadState[1] & DPAD_DOWN) &&
                   ForgingBook_IsGridPositionValid(book->column, book->row + 1, book->page)) {
            book->row++;
            moved = 1;
            Sound_PlaySong(0xdc);
        } else if ((gPadState[1] & DPAD_LEFT) &&
                   ForgingBook_IsGridPositionValid(book->column - 1, book->row, book->page)) {
            book->column--;
            moved = 1;
            Sound_PlaySong(0xdc);
        } else if ((gPadState[1] & DPAD_RIGHT) &&
                   ForgingBook_IsGridPositionValid(book->column + 1, book->row, book->page)) {
            book->column++;
            moved = 1;
            Sound_PlaySong(0xdc);
        }
    }
    if (pageChanged || moved)
        book->selectedEntry = book->page * 10 + book->row * 5 + book->column;
    if (pageChanged)
        ForgingBook_RefreshPage(book);
    if (moved)
        sub_0821353C(book);
    if (book->exiting) {
        if (book->fadeTimer > 1 << book->fadeShift) {
            if (book->exitScript) {
                values[0] = book->trackedEntry;
                arg.type = 2;
                arg.value = values;
                Script_Call(book->exitScript, &arg);
            }
            Task_Kill(book);
        }
        book->fadeTimer++;
    } else {
        ForgingBook_UpdateCursorPulse(book);
    }
}
