#include "global.h"

struct S {
    u8 pad[0x2e2];
    u8 f2e2;
};
u8 ChallengeMenu_ConsumeStateEntry(struct S *);
void sub_081C569C(struct S *);

void ChallengeMenu_UpdateSelection(struct S *p)
{
    if (ChallengeMenu_ConsumeStateEntry(p))
        p->f2e2 = 0;
    sub_081C569C(p);
}
