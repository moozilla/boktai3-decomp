#include "global.h"
#define ForgingBook_InitializeFromScript sub_08212D84
#define ForgingBook_InitSprites sub_082124D8
#define ForgingBook_RefreshCategory sub_08212324
#define gForgingBookWeaponEntries gUnk_08613FE8
#define gForgingBookItemEntries gUnk_08614019

struct ForgingBook {
    u8 unknown[0x20];
    u32 state;
    u8 unknown24[0xB3C];
    s16 category;
    s16 selection;
    u8 unknownB64[4];
    s16 unknownB68;
    u8 unknownB6A[2];
    s16 unknownB6C;
    s16 transition;
    u8 unknownB70[4];
    s32 textWindow;
    u32 nameText;
    u32 helpText;
    u32 alternateNameText;
    u8 entries[0x61];
    u8 unknownBE5;
    u8 unknownBE6;
    u8 unknownBE7;
    s32 scriptValue;
};
extern const u8 gUnk_08613FE8[], gUnk_08614019[];
void ForgingBook_InitBackgrounds(struct ForgingBook *);
void sub_082124D8(struct ForgingBook *);
s32 sub_08212324(struct ForgingBook *);
s32 Script_SeekToKeyword(u32);
u32 Script_GetTextSourcePc(void);
s32 Script_GetKeywordValue(u32, s32);
int TextWindow_Create(int, int, int, int);
void ScrollingDialogue_RequestShow(void);
u32 ScrollingDialogue_ClearScript(void);
void ScreenTransition_SendCommand(s32,s32,s32,s32,s32,s32,s32);
void Sound_PlaySong(u32);

s32 sub_08212D84(struct ForgingBook *book)
{
    s32 i, j;
    book->state = 0;
    book->category = 0;
    book->selection = 0;
    book->transition = 4;
    book->unknownB6C = 0;
    book->unknownB68 = 0;
    book->unknownBE6 = 5;
    book->unknownBE7 = 1;
    for (i = 0; i <= 0x30; i++)
        book->entries[i] = gUnk_08613FE8[i];
    for (j = 0, i = 0x31; i <= 0x60; i++, j++)
        book->entries[i] = gUnk_08614019[j];
    ForgingBook_InitBackgrounds(book);
    sub_082124D8(book);
    if (!Script_SeekToKeyword('n'))
        goto fail;
    book->nameText = Script_GetTextSourcePc();
    if (!book->nameText)
        goto fail;
    if (!Script_SeekToKeyword('h'))
        goto fail;
    book->helpText = Script_GetTextSourcePc();
    if (!book->helpText)
        goto fail;
    if (!Script_SeekToKeyword('N'))
        goto fail;
    book->alternateNameText = Script_GetTextSourcePc();
    if (book->alternateNameText)
        goto ready;
fail:
    return -1;
ready:
    book->scriptValue = Script_GetKeywordValue('e', 0);
    book->textWindow = TextWindow_Create(1, 16, 29, 4);
    ScrollingDialogue_RequestShow();
    ScrollingDialogue_ClearScript();
    sub_08212324(book);
    ScreenTransition_SendCommand(0, book->transition, 4, 4, 4, 0xFFFF, 0);
    Sound_PlaySong(280);
    return 0;
}
