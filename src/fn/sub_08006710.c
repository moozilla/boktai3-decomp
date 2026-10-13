#include "choice_menu.h"

void sub_082195E0(void *);
u32 sub_08006710(struct ChoiceMenu *p)
{
    sub_082195E0(&p->leftCursor);
    sub_082195E0(&p->rightCursor);
    return gUnk_0200002C = 0;
}
