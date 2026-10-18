#define ForgingBook_InitializeFromScript sub_08213B64
#include "global.h"

struct ForgingBook {
    u8 unk0[0x20];
    u32 state;
    u8 unk24[0x6F4 - 0x24];
    s16 page;
    s16 unk6F6;
    u8 unk6F8[4];
    s16 selection;
    u8 unk6FE[2];
    s16 unk700;
    s16 transition;
    u8 unk704[2];
    s16 optionC;
    u32 textWindow;
    u32 windowScript;
    u32 collectedText;
    u32 uncollectedText;
    s32 optionE;
    u8 unk71C[6];
    s16 optionS;
};

void ForgingBook_SetupPageBackground(struct ForgingBook *);
void sub_0821358C(struct ForgingBook *);
s32 Script_SeekToKeyword(u32);
u32 Script_GetTextSourcePc(void);
s32 Script_GetValue(void);
s32 Script_GetKeywordValue(u32, s32);
void sub_0821304C(struct ForgingBook *);
void CollectionBook_RefreshCompletionDigits(struct ForgingBook *);
void CollectionBook_RefreshPageHeading(struct ForgingBook *);
void CollectionBook_UpdateTrackedEntryMarker(struct ForgingBook *);
int TextWindow_Create(int, int, int, int);
void ScrollingDialogue_RequestShow(void);
u32 ScrollingDialogue_ClearScript(void);
u32 ForgingBook_RefreshEntryDescription(struct ForgingBook *);
void ScreenTransition_SendCommand(s32, s32, s32, s32, s32, s32, s32);
void Sound_PlaySong(u32);

s32 sub_08213B64(struct ForgingBook *book, u32 arg)
{
    book->state = 0;
    book->page = 0;
    book->unk6F6 = 0;
    book->transition = 4;
    book->unk700 = 0;
    book->selection = 0;
    ForgingBook_SetupPageBackground(book);
    sub_0821358C(book);
    if (!Script_SeekToKeyword('m'))
        return -1;
    book->collectedText = Script_GetTextSourcePc();
    if (!book->collectedText)
        return -1;
    if (!Script_SeekToKeyword('n'))
        return -1;
    book->windowScript = Script_GetTextSourcePc();
    if (!book->windowScript)
        return -1;
    if (!Script_SeekToKeyword('h'))
        return -1;
    book->uncollectedText = Script_GetTextSourcePc();
    if (!book->uncollectedText)
        return -1;
    if (Script_SeekToKeyword('s'))
        book->optionS = Script_GetValue();
    else
        book->optionS = 0;
    if (Script_SeekToKeyword('c'))
        book->optionC = Script_GetValue();
    else
        book->optionC = 0;
    book->optionE = Script_GetKeywordValue('e', 0);
    sub_0821304C(book);
    CollectionBook_RefreshCompletionDigits(book);
    CollectionBook_RefreshPageHeading(book);
    CollectionBook_UpdateTrackedEntryMarker(book);
    book->textWindow = TextWindow_Create(1, 16, 29, 4);
    ScrollingDialogue_RequestShow();
    ScrollingDialogue_ClearScript();
    ForgingBook_RefreshEntryDescription(book);
    ScreenTransition_SendCommand(0, book->transition, 4, 4, 4, 0xFFFF, 0);
    Sound_PlaySong(0x118);
    return 0;
}
