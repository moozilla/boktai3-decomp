#include "global.h"

struct S {
    u8 pad[0x310];
    s32 f310;
};
void ChallengeMenu_SetState(struct S *, void (*)(void));
void ChallengeMenu_UpdateSelection(void);

void ChallengeMenu_UpdateFadeIn(struct S *p)
{
    p->f310++;
    if (p->f310 > 0x20)
        ChallengeMenu_SetState(p, ChallengeMenu_UpdateSelection);
}
