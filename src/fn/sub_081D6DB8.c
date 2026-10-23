#include "global.h"
#include "monster_book.h"

static inline void Sprite_Hide(struct BookSprite *s)
{
    s->flags |= 1;
}

void sub_081D6DB8(struct MonsterBook *p)
{
    s32 j;

    for (j = 0; j < 2; j++) {
        p->sA8C[j].flags |= 1;
        p->sBAC[j].flags |= 1;
    }
    Sprite_Hide(&p->sB4C);
}
