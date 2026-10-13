#include "choice_menu.h"
void sub_0822B2F8(s32);
void ChoiceMenu_Select(struct ChoiceMenu *p, s32 selection)
{
    struct Choice *choice;
    if (selection >= 0) {
        if (selection >= p->count)
            selection = p->count - 1;
        choice = &p->choices[selection];
        p->leftCursor.flags &= ~512;
        p->rightCursor.flags &= ~512;
        p->leftCursor.x = choice->x - 7;
        p->leftCursor.y = choice->y;
        p->rightCursor.x = choice->x + 1 + (choice->width << 3);
        p->rightCursor.y = choice->y;
    } else {
        choice = &p->choices[p->initial];
        p->leftCursor.flags |= 512;
        p->rightCursor.flags |= 512;
        p->leftCursor.x = choice->x - 7;
        p->leftCursor.y = choice->y;
        p->rightCursor.x = choice->x + 1 + (choice->width << 3);
        p->rightCursor.y = choice->y;
    }
    if (selection != p->selected)
        sub_0822B2F8(220);
    p->selected = selection;
}
