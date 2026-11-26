#include "global.h"
u32 Script_ParseStringRef(u32);
u8 *Text_LookupString(u32);
void ScrollingDialogue_SetScript(u8 *);
void ForgingBook_SetItemNameText(u8 *p, u32 q)
{
    u32 r = Script_ParseStringRef(q) + 0x30;
    ScrollingDialogue_SetScript(Text_LookupString(r + *p));
}
