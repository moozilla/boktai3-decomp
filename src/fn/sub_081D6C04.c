#include "global.h"
#include "monster_book.h"

void sub_081D6C04(struct MonsterBook *p)
{
    s32 i, j;

    for (i = 0; i < 7; i++) {
        for (j = 0; j < 2; j++)
            p->s54C[i][j].flags |= 1;
        p->s2AC[i].flags |= 1;
    }
}
