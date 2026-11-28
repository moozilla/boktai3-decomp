#include "global.h"
#include "ui/books/collection_book.h"
void ForgingBook_SetupPageBackground(struct CollectionBook *);
void sub_0821358C(struct CollectionBook *);
void CollectionBook_RefreshCollectedEntryVisibility(struct CollectionBook *);
void CollectionBook_RefreshCompletionDigits(struct CollectionBook *);
void CollectionBook_RefreshPageHeading(struct CollectionBook *);
void CollectionBook_UpdateTrackedEntryMarker(struct CollectionBook *);
u32 TextWindow_Create(int, int, int, int);
void ScrollingDialogue_RequestShow(void);
void ScrollingDialogue_ClearScript(void);
void ForgingBook_RefreshEntryDescription(struct CollectionBook *);
void ScreenTransition_SendCommand(s32, s32, s32, s32, s32, s32, s32);
void Sound_PlaySong(u32);

s32 ForgingBook_InitializePageFromScript(struct CollectionBook *book, u32 arg)
{
    book->unknown20 = 0;
    book->page = 0;
    book->unknown6F6 = 0;
    book->fadeShift = 4;
    book->exiting = 0;
    book->selectedEntry = 0;
    ForgingBook_SetupPageBackground(book);
    sub_0821358C(book);
    if (!Script_SeekToKeyword('m') || !(book->descriptionScript = Script_GetTextSourcePc()))
        return -1;
    if (!Script_SeekToKeyword('n') || !(book->nameScript = Script_GetTextSourcePc()))
        return -1;
    if (!Script_SeekToKeyword('h') || !(book->lockedDescriptionScript = Script_GetTextSourcePc()))
        return -1;
    if (Script_SeekToKeyword('s'))
        book->trackedEntry = Script_GetValue();
    else
        book->trackedEntry = 0;
    if (Script_SeekToKeyword('c'))
        book->trackingLocked = Script_GetValue();
    else
        book->trackingLocked = 0;
    book->exitScript = Script_GetKeywordValue('e', 0);
    CollectionBook_RefreshCollectedEntryVisibility(book);
    CollectionBook_RefreshCompletionDigits(book);
    CollectionBook_RefreshPageHeading(book);
    CollectionBook_UpdateTrackedEntryMarker(book);
    book->textWindow = TextWindow_Create(1, 16, 29, 4);
    ScrollingDialogue_RequestShow();
    ScrollingDialogue_ClearScript();
    ForgingBook_RefreshEntryDescription(book);
    ScreenTransition_SendCommand(0, book->fadeShift, 4, 4, 4, 0xffff, 0);
    Sound_PlaySong(0x118);
    return 0;
}
