#include "global.h"
struct SceneTextSelector {
    u8 unknown0[0x2e3];
    u8 enabledChoices;
    u8 unknown2E4[0x2fc - 0x2e4];
    s32 group;
    s32 choice;
    const void *textRef;
};
s32 Script_ParseStringRef(const void *);
u8 *Text_LookupString(s32);
u32 ScrollingDialogue_ClearScript(void);
s32 ScrollingDialogue_SetScript(const u8 *);
void ScrollingDialogue_RequestShow(void);
void ScrollingDialogue_HideImmediately(void);
void SceneTextSelector_UpdatePanel(struct SceneTextSelector *p)
{
    if (p->choice == 0)
        goto close;
    if (p->choice > 3)
        return;
    if (!((p->enabledChoices >> (p->choice - 1)) & 1)) {
    close:
        ScrollingDialogue_ClearScript();
        ScrollingDialogue_HideImmediately();
    } else {
        ScrollingDialogue_ClearScript();
        {
            s32 id = Script_ParseStringRef(p->textRef) + p->group * 3;
            id--;
            id += p->choice;
            ScrollingDialogue_SetScript(Text_LookupString(id));
        }
        ScrollingDialogue_RequestShow();
    }
}
