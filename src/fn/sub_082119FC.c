#include "global.h"
u32 Script_ParseStringRef(u32);
void Text_LookupString(u32);
void ScrollingDialogue_SetScript(void);
void ForgingBook_SetItemNameText(u8 *p, u32 q)
{
    u32 r = Script_ParseStringRef(q) + 0x30;
    Text_LookupString(r + *p);
    ScrollingDialogue_SetScript();
}
