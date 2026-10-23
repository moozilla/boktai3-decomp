#include "global.h"
#include "monster_book.h"

s32 ThresholdTrack_ClassifyPosition(u8);
s32 Bestiary_CountUnlockedGroupEntries(s32);
s32 Bestiary_GetGroupEntryCount(s32);
void Anim_DrawDecimalDigits(void *, void *, int, int, int, int, int, int, int);

static inline void Sprite_Hide(struct BookSprite *s)
{
    s->flags |= 1;
}

static inline void Sprite_Show(struct BookSprite *s)
{
    s->flags &= ~1;
}

static inline void MonsterBook_HideGroupDigits(struct MonsterBook *p)
{
    s32 j;

    for (j = 0; j < 2; j++) {
        p->sA8C[j].flags |= 1;
        p->sBAC[j].flags |= 1;
    }
    Sprite_Hide(&p->sB4C);
}

void sub_081D6E0C(struct MonsterBook *p)
{
    s32 group;

    MonsterBook_HideGroupDigits(p);
    group = ThresholdTrack_ClassifyPosition(p->entry);
    Anim_DrawDecimalDigits(p->sA8C, p->animData, Bestiary_CountUnlockedGroupEntries(group), 2, 0,
                           0x98, 0x16, 8, 0);
    Sprite_Show(&p->sB4C);
    p->sB4C.x = 0xA8;
    p->sB4C.y = 0x16;
    Anim_DrawDecimalDigits(p->sBAC, p->animData, Bestiary_GetGroupEntryCount(group), 2, 0, 0xB0,
                           0x16, 8, 0);
}
