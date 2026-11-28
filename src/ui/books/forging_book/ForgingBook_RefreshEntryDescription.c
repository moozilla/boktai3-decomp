#include "global.h"
#include "ui/books/collection_book.h"
void TextWindow_StartPlayback(s32);
void ScrollingDialogue_ClearScript(void);
s32 ScrollingDialogue_SetScript(const u8 *);

u32 ForgingBook_RefreshEntryDescription(struct CollectionBook *book)
{
    s32 id;

    if (CollectionBook_IsEntryCollected(book->entryIds[book->selectedEntry])) {
        TextWindow_SetScript(book->textWindow, book->nameScript);
        TextWindow_Refresh(book->textWindow, book->entryIds[book->selectedEntry]);
        TextWindow_StartPlayback(book->textWindow);
        ScrollingDialogue_ClearScript();
        id = Script_ParseStringRef(book->descriptionScript);
        ScrollingDialogue_SetScript(Text_LookupString(id + book->entryIds[book->selectedEntry]));
    } else {
        TextWindow_SetScript(book->textWindow, book->nameScript);
        TextWindow_Refresh(book->textWindow, 10);
        TextWindow_StartPlayback(book->textWindow);
        ScrollingDialogue_ClearScript();
        id = Script_ParseStringRef(book->lockedDescriptionScript);
        ScrollingDialogue_SetScript(Text_LookupString(id + book->entryIds[book->selectedEntry]));
    }
    return 0;
}
