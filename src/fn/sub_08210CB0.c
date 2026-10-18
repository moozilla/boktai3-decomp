#include "global.h"
s32 Collection_IsEntryUnlocked(s32);
void TextWindow_SetScript(void *, void *);
void TextWindow_Refresh(void *, s32);
void TextWindow_StartPlayback(void *);
void ScrollingDialogue_ClearScript(void);
void ScrollingDialogue_SetScript(void *);
u32 Script_ParseStringRef(void *);
void *Text_LookupString(u32);
u32 Collection_UpdateSelectedEntryText(u8 *p)
{
    s16 *idx = (s16 *)(p + 0xb88);
    if (Collection_IsEntryUnlocked(*idx)) {
        void **obj = (void **)(p + 0xb94);
        u32 v;
        s32 t;
        TextWindow_SetScript(*obj, *(void **)(p + 0xb98));
        TextWindow_Refresh(*obj, *idx);
        TextWindow_StartPlayback(*obj);
        ScrollingDialogue_ClearScript();
        v = Script_ParseStringRef(*(void **)(p + 0xb9c));
        t = *idx;
        t = v - -t; /* keeps the add's operand order (v + t) */
        ScrollingDialogue_SetScript(Text_LookupString(t));
    } else {
        void **obj = (void **)(p + 0xb94);
        u32 v;
        s32 t;
        TextWindow_SetScript(*obj, *(void **)(p + 0xb98));
        TextWindow_Refresh(*obj, 27);
        TextWindow_StartPlayback(*obj);
        ScrollingDialogue_ClearScript();
        v = Script_ParseStringRef(*(void **)(p + 0xba0));
        t = *idx;
        t = v - -t; /* keeps the add's operand order (v + t) */
        ScrollingDialogue_SetScript(Text_LookupString(t));
    }
    return 0;
}
