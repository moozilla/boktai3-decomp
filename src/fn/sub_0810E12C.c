#define SelectionPanel_UpdateDescription sub_0810E12C
#define SelectionPanel_GetSelection sub_0810E1A0
#include "global.h"
struct SelectionPanel {
    u8 unknown0[0xCF0];
    const void *descriptionText;
    u8 unknownCF4[0x11];
    s8 lastDescription;
};
s32 sub_0810E1A0(struct SelectionPanel *);
u32 ScrollingDialogue_ClearScript(void);
u8 sub_0811DAD4(s32);
s32 Script_ParseStringRef(const void *);
u8 *Text_LookupString(s32);
s32 ScrollingDialogue_SetScript(const u8 *);
void sub_0810E12C(struct SelectionPanel *p)
{
    s32 selection = sub_0810E1A0(p);
    if (selection < 0)
        selection = 0;
    if (p->lastDescription != selection) {
        ScrollingDialogue_ClearScript();
        if (sub_0811DAD4(selection)) {
            ScrollingDialogue_SetScript(Text_LookupString(Script_ParseStringRef(p->descriptionText) + selection));
        } else {
            ScrollingDialogue_SetScript(Text_LookupString(Script_ParseStringRef(p->descriptionText) + 12));
        }
        p->lastDescription = selection;
    }
}
