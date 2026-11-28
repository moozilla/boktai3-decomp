#include "global.h"
#include "ui/books/forging_book.h"
extern const u8 gUnk_08613FE8[], gUnk_08614019[];
void ForgingBook_InitBackgrounds(struct ForgingBook *);
void ForgingBook_InitSprites(struct ForgingBook *);
s32 ForgingBook_RefreshCategory(struct ForgingBook *);
u32 TextWindow_Create(int, int, int, int);
void ScrollingDialogue_RequestShow(void);
void ScrollingDialogue_ClearScript(void);
void ScreenTransition_SendCommand(s32, s32, s32, s32, s32, s32, s32);
void Sound_PlaySong(u32);

s32 ForgingBook_InitializeFromScript(struct ForgingBook *book)
{
    s32 i, j;

    book->frame = 0;
    book->category = 0;
    book->unknownB62 = 0;
    book->fadeShift = 4;
    book->exiting = 0;
    book->selectedEntry = 0;
    book->unknownBE6 = 5;
    book->unknownBE7 = 1;
    for (i = 0; i <= 0x30; i++)
        book->entries[i] = gUnk_08613FE8[i];
    for (j = 0, i = 0x31; i <= 0x60; i++, j++)
        book->entries[i] = gUnk_08614019[j];
    ForgingBook_InitBackgrounds(book);
    ForgingBook_InitSprites(book);
    if (!Script_SeekToKeyword('n') || !(book->nameScript = Script_GetTextSourcePc()))
        return -1;
    if (!Script_SeekToKeyword('h') || !(book->helpScript = Script_GetTextSourcePc()))
        return -1;
    if (!Script_SeekToKeyword('N') || !(book->itemNameScript = Script_GetTextSourcePc()))
        return -1;
    book->exitScript = Script_GetKeywordValue('e', 0);
    book->textWindow = TextWindow_Create(1, 16, 29, 4);
    ScrollingDialogue_RequestShow();
    ScrollingDialogue_ClearScript();
    ForgingBook_RefreshCategory(book);
    ScreenTransition_SendCommand(0, book->fadeShift, 4, 4, 4, 0xffff, 0);
    Sound_PlaySong(280);
    return 0;
}
