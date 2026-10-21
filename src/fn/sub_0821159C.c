#include "global.h"
void Collection_LoadPageBackground(void *);
void CollectionMenu_InitGraphics(void *);
s32 Script_SeekToKeyword(s32);
void *Script_GetTextSourcePc(void);
void *Script_GetKeywordValue(s32, s32);
void Collection_UpdateEntryVisibility(void *);
void Collection_UpdateCountDigits(void *);
void Collection_UpdatePageIndicator(void *);
void *TextWindow_Create(u32, u32, u32, u32);
void ScrollingDialogue_RequestShow(void);
void ScrollingDialogue_ClearScript(void);
u32 Collection_UpdateSelectedEntryText(void *);
void ScreenTransition_SendCommand(s32, s32, s32, s32, s32, s32, s32);
void Sound_PlaySong(u32);
s32 CollectionMenu_Init(u8 *s)
{
    u32 zero = 0, four;
    s16 *p;
    void *v;
    *(u32 *)(s + 0x20) = zero;
    *(u16 *)(s + 0xb80) = zero;
    *(u16 *)(s + 0xb82) = zero;
    p = (s16 *)(s + 0xb8e);
    four = 4;
    *p = four;
    *(u16 *)(s + 0xb8c) = zero;
    *(u16 *)(s + 0xb88) = zero;
    Collection_LoadPageBackground(s);
    CollectionMenu_InitGraphics(s);
    if (!Script_SeekToKeyword(0x6d))
        goto fail;
    if (!(*(void **)(s + 0xb9c) = Script_GetTextSourcePc()))
        goto fail;
    if (!Script_SeekToKeyword(0x6e))
        goto fail;
    if (!(*(void **)(s + 0xb98) = Script_GetTextSourcePc()))
        goto fail;
    if (!Script_SeekToKeyword(0x68))
        goto fail;
    if ((*(void **)(s + 0xba0) = Script_GetTextSourcePc()))
        goto success;
fail:
    return -1;
success:
    *(void **)(s + 0xba4) = Script_GetKeywordValue(0x65, 0);
    Collection_UpdateEntryVisibility(s);
    Collection_UpdateCountDigits(s);
    Collection_UpdatePageIndicator(s);
    *(void **)(s + 0xb94) = TextWindow_Create(1, 0x10, 0x1d, 4);
    ScrollingDialogue_RequestShow();
    ScrollingDialogue_ClearScript();
    Collection_UpdateSelectedEntryText(s);
    ScreenTransition_SendCommand(0, *p, 4, 4, four, 0xffff, zero);
    Sound_PlaySong(0x118);
    return 0;
}
