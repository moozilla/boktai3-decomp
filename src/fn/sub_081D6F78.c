#include "global.h"
#include "monster_book.h"

static inline void Sprite_Hide(struct BookSprite *s)
{
    s->flags |= 1;
}

void sub_081D6F78(struct MonsterBook *p)
{
    Sprite_Hide(&p->s18C);
    Sprite_Hide(&p->s1EC);
}
