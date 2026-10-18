#include "global.h"

struct S {
    u8 pad[0x310];
    s32 f310;
};
void ChallengeMenu_SetState(struct S *, void (*)(void));
void ScrollingDialogue_ClearScript(void);
void ScrollingDialogue_HideImmediately(void);
void ChallengeMenu_DispatchSelection(void);

void ChallengeMenu_UpdateFadeOut(struct S *p)
{
    p->f310++;
    if (p->f310 > 0x20) {
        ScrollingDialogue_ClearScript();
        ScrollingDialogue_HideImmediately();
        ChallengeMenu_SetState(p, ChallengeMenu_DispatchSelection);
    }
}
