#include "global.h"
#include "ui/books/collection.h"

void CollectionMenu_InitGraphics(struct CollectionMenu *menu);
void ScrollingDialogue_RequestShow(void);
void Sound_PlaySong(u32);

s32 CollectionMenu_Init(struct CollectionMenu *menu)
{
    menu->unknown20 = 0;
    menu->page = 0;
    menu->unknownB82 = 0;
    menu->fadeShift = 4;
    menu->exiting = 0;
    menu->selectedEntry = 0;
    Collection_LoadPageBackground(menu);
    CollectionMenu_InitGraphics(menu);
    if (!Script_SeekToKeyword('m') || !(menu->descriptionScript = Script_GetTextSourcePc()))
        return -1;
    if (!Script_SeekToKeyword('n') || !(menu->nameScript = Script_GetTextSourcePc()))
        return -1;
    if (!Script_SeekToKeyword('h') || !(menu->lockedDescriptionScript = Script_GetTextSourcePc()))
        return -1;
    menu->exitScript = Script_GetKeywordValue('e', 0);
    Collection_UpdateEntryVisibility(menu);
    Collection_UpdateCountDigits(menu);
    Collection_UpdatePageIndicator(menu);
    menu->textWindow = TextWindow_Create(1, 0x10, 0x1d, 4);
    ScrollingDialogue_RequestShow();
    ScrollingDialogue_ClearScript();
    Collection_UpdateSelectedEntryText(menu);
    ScreenTransition_SendCommand(0, menu->fadeShift, 4, 4, 4, 0xffff, 0);
    Sound_PlaySong(0x118);
    return 0;
}
