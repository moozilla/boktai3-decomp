#include "global.h"
u32 Script_ParseStringRef(u32);
u32 Text_LookupString(u32);
void MessageWindow_SetMessageTable(u32);
void MessageWindow_ShowMessage(u32);
void ScrollingDialogue_SetNumberArgument(u32, u32);
void ScrollingDialogue_SetScript(u32);
void Presentation_ShowItemDescription(u8 *p, u32 q)
{
    u32 s = Script_ParseStringRef(q) + 0x30;
    MessageWindow_SetMessageTable(q);
    MessageWindow_ShowMessage(*p);
    ScrollingDialogue_SetNumberArgument(0, 0);
    ScrollingDialogue_SetNumberArgument(1, 999);
    ScrollingDialogue_SetScript(Text_LookupString(s + *p));
}
