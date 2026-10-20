#include "global.h"

void ScrollingDialogue_ClearScript(void);
void ScrollingDialogue_HideImmediately(void);
void ScrollingDialogue_RequestShow(void);
s32 Script_ParseStringRef(const void *);
u8 *Text_LookupString(s32);
void ScrollingDialogue_SetScript(u8 *);

void ScrollingDialogue_Step(u8 *p)
{
    u16 *flags = (u16 *)(p + 0xD4E);
    if (*flags & 1) {
        if (*flags & 2) {
            ScrollingDialogue_ClearScript();
            ScrollingDialogue_HideImmediately();
            *flags = (*flags & ~2) | 4;
        }
    } else if (*flags & 8) {
        if (*flags & 4) {
            ScrollingDialogue_ClearScript();
            ScrollingDialogue_RequestShow();
            *flags &= ~4;
        }
        ScrollingDialogue_ClearScript();
        ScrollingDialogue_SetScript(
            Text_LookupString(Script_ParseStringRef(*(void **)(p + 0xF10)) + p[0x26]));
        *flags = (*flags & 0xFFF7) | 2;
        *flags &= 0xFFFB;
    }
}
