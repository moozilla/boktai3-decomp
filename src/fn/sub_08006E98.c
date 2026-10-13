#include "choice_menu.h"

s32 Script_SeekToKeyword(s32);
s32 Script_GetValue(void);
void *sub_08227E90(void);
void sub_08006E98(void)
{
    struct ChoiceMenu *p = gUnk_0200002C;
    if (p != 0 && Script_SeekToKeyword(0x72)) {
        p->window = sub_08227E90();
        p->stringBase = sub_0821ABA8(0x69, 0);
        if (Script_SeekToKeyword(0x77))
            p->selected = Script_GetValue();
        if (p->window != 0) {
            ChoiceMenu_LayoutText(p, p->window);
            ChoiceMenu_Select(p, p->selected);
            sub_08006738(p);
        }
    }
}
