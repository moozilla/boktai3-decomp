#include "choice_menu.h"

void sub_08030F20(u32, u32, u32, u32);
void sub_08030B78(u32);
void sub_08030C84(u32);
void sub_08030DBC(u32);

void sub_08006738(struct ChoiceMenu *s)
{
    sub_08030F20(s->left, s->top, s->maxWidth, s->maxRows);
    sub_08030B78(1);
    sub_08030C84((u32)s->window);
    sub_08030DBC(s->stringBase);
}
