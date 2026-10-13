#include "choice_menu.h"

s32 sub_08006478(struct ChoiceMenu *p, s32 a, s32 d)
{
    s32 x = a;
    s32 i;
    for (i = 0; i < p->rows; i++) {
        x += d;
        if (x > p->rows)
            x = p->rows;
        else if (x < 0)
            x = 0;
        if (p->rowCount[x] != 0)
            return x;
    }
    return a;
}
