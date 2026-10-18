#include "global.h"
struct SelectionPanel {
    u8 unknown0[0xCF0];
    const void *descriptionText;
    u8 unknownCF4[0x11];
    s8 lastDescription;
};
s32 SelectionPanel_GetSelection(struct SelectionPanel *);
u32 ScrollingDialogue_ClearScript(void);
u8 StateFlags_TestIndexedFlag(s32);
s32 Script_ParseStringRef(const void *);
u8 *Text_LookupString(s32);
s32 ScrollingDialogue_SetScript(const u8 *);
void SelectionPanel_UpdateDescription(struct SelectionPanel *p)
{
    s32 selection = SelectionPanel_GetSelection(p);
    if (selection < 0)
        selection = 0;
    if (p->lastDescription != selection) {
        ScrollingDialogue_ClearScript();
        if (StateFlags_TestIndexedFlag(selection)) {
            ScrollingDialogue_SetScript(Text_LookupString(Script_ParseStringRef(p->descriptionText) + selection));
        } else {
            ScrollingDialogue_SetScript(Text_LookupString(Script_ParseStringRef(p->descriptionText) + 12));
        }
        p->lastDescription = selection;
    }
}
