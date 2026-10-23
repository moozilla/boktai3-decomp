#include "global.h"
#include "monster_book.h"

void Anim_DrawDecimalDigits(void *, void *, int, int, int, int, int, int, int);

static inline void MonsterBook_HideRows(struct MonsterBook *p)
{
    s32 i, j;

    for (i = 0; i < 7; i++) {
        for (j = 0; j < 2; j++)
            p->s54C[i][j].flags |= 1;
        p->s2AC[i].flags |= 1;
    }
}

void sub_081D6D20(struct MonsterBook *p)
{
    MonsterBook_HideRows(p);
    Anim_DrawDecimalDigits(p->s54C[0], p->animData, p->entry + 1, 2, 0, 0x28, 0x18, 8, 0);
}
