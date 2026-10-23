#include "global.h"
#include "monster_book.h"

s32 Bestiary_GetCategoryFirstEntry(u32);
s32 Bestiary_GetCategoryEntryCount(u32);
void Anim_DrawDecimalDigits(void *, void *, int, int, int, int, int, int, int);

static inline void Sprite_Show(struct BookSprite *s)
{
    s->flags &= ~1;
}

static inline void MonsterBook_HideRows(struct MonsterBook *p)
{
    s32 i, j;

    for (i = 0; i < 7; i++) {
        for (j = 0; j < 2; j++)
            p->s54C[i][j].flags |= 1;
        p->s2AC[i].flags |= 1;
    }
}

void sub_081D6C4C(struct MonsterBook *p)
{
    s32 i;
    s32 count, first;

    MonsterBook_HideRows(p);
    count = Bestiary_GetCategoryEntryCount(p->category);
    first = Bestiary_GetCategoryFirstEntry(p->category) + 1;
    for (i = 0; i < count; i++) {
        Anim_DrawDecimalDigits(p->s54C[i], p->animData, first + i, 2, 0, 0x18, i * 16 + 0x28, 8, 0);
        Sprite_Show(&p->s2AC[i]);
        p->s2AC[i].x = 0x28;
        p->s2AC[i].y = i * 16 + 0x28;
    }
}
