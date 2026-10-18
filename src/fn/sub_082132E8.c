#include "global.h"
u32 CollectionBook_IsEntryCollected(s32);
void TextWindow_SetScript(u32, u32);
void TextWindow_Refresh(u32, s32);
void TextWindow_StartPlayback(u32);
u32 ForgingBook_RefreshEntryDescription(u8 *s)
{
    s16 *index = (s16 *)(s + 0x6FC);
    s32 offset = *index * 2;
    s16 *entries = (s16 *)(s + 0x6E0);
    u32 *handle;
    u32 ref;
    if (CollectionBook_IsEntryCollected(*(s16 *)((u8 *)entries + offset))) {
        handle = (u32 *)(s + 0x708);
        TextWindow_SetScript(*handle, *(u32 *)(s + 0x70C));
        TextWindow_Refresh(*handle, *(s16 *)((u8 *)entries + (*index << 1)));
        TextWindow_StartPlayback(*handle);
        ScrollingDialogue_ClearScript();
        ref = Script_ParseStringRef(*(u32 *)(s + 0x710));
        ScrollingDialogue_SetScript(Text_LookupString(ref + *(s16 *)((u8 *)entries + (*index << 1))));
    } else {
        handle = (u32 *)(s + 0x708);
        TextWindow_SetScript(*handle, *(u32 *)(s + 0x70C));
        TextWindow_Refresh(*handle, 10);
        TextWindow_StartPlayback(*handle);
        ScrollingDialogue_ClearScript();
        ref = Script_ParseStringRef(*(u32 *)(s + 0x714));
        offset = *index * 2;
        ScrollingDialogue_SetScript(Text_LookupString(ref + *(s16 *)((u32)entries + offset)));
    }
    return 0;
}
