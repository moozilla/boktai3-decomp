#include "global.h"
#include "ui/books/forging_book.h"
void TextWindow_SetString(s32, u32, const u8 *);
void TextWindow_StartPlayback(s32);
void ScrollingDialogue_ClearScript(void);
void Presentation_ApplyPackedGraphicData(const void *, const void *);
void ForgingBook_SetItemNameText(const u8 *, const void *);
extern const u8 gUnk_08E6092C[], gUnk_08E60F80[];

u32 ForgingBook_RefreshSelectedEntryText(struct ForgingBook *book)
{
    if (book->category <= 5) {
        if (ForgingBook_IsWeaponDiscovered(book->entries[book->selectedEntry])) {
            TextWindow_SetScript(book->textWindow, book->nameScript);
            book->emptyString[0] = 0;
            TextWindow_Refresh(book->textWindow, book->entries[book->selectedEntry]);
            TextWindow_SetString(book->textWindow, 0, book->emptyString);
            TextWindow_StartPlayback(book->textWindow);
            ScrollingDialogue_ClearScript();
            Presentation_ApplyPackedGraphicData(
                gUnk_08E6092C + book->entries[book->selectedEntry] * 32, book->nameScript);
        } else {
            TextWindow_SetScript(book->textWindow, book->nameScript);
            book->emptyString[0] = 0;
            TextWindow_Refresh(book->textWindow, 0x62);
            TextWindow_SetString(book->textWindow, 0, book->emptyString);
            TextWindow_StartPlayback(book->textWindow);
            ScrollingDialogue_ClearScript();
        }
    } else {
        if (ForgingBook_IsItemDiscoveryRecorded(book->entries[book->selectedEntry])) {
            TextWindow_SetScript(book->textWindow, book->itemNameScript);
            book->emptyString[0] = 0;
            TextWindow_Refresh(book->textWindow, book->entries[book->selectedEntry]);
            TextWindow_SetString(book->textWindow, 0, book->emptyString);
            TextWindow_StartPlayback(book->textWindow);
            ScrollingDialogue_ClearScript();
            ForgingBook_SetItemNameText(gUnk_08E60F80 + book->entries[book->selectedEntry] * 16,
                                        book->itemNameScript);
        } else {
            TextWindow_SetScript(book->textWindow, book->nameScript);
            book->emptyString[0] = 0;
            TextWindow_Refresh(book->textWindow, 0x62);
            TextWindow_SetString(book->textWindow, 0, book->emptyString);
            TextWindow_StartPlayback(book->textWindow);
            ScrollingDialogue_ClearScript();
        }
    }
    return 0;
}
