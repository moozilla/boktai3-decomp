#include "global.h"
#include "monster_book.h"


static inline void Sprite_Hide(struct SceneInputWidget *s)
{
    s->flags |= 1;
}

static inline void Sprite_Show(struct SceneInputWidget *s)
{
    s->flags &= ~1;
}

static inline void MonsterBook_HideCategoryBadge(struct MonsterBook *p)
{
    Sprite_Hide(&p->s18C);
    Sprite_Hide(&p->s1EC);
}

void sub_081D6F98(struct MonsterBook *p)
{
    s32 frame, value;
    s32 x, y;

    MonsterBook_HideCategoryBadge(p);
    x = 0;
    y = 0;
    switch (p->category) {
        case 0:
        case 1:
        case 2:
            frame = 0;
            value = p->category;
            x = 0x70;
            y = 0x16;
            break;
        case 3:
        case 4:
            frame = 1;
            value = p->category - 3;
            x = 0x80;
            y = 0x16;
            break;
        case 5:
        case 6:
        case 7:
        case 8:
        case 9:
        case 10:
            frame = 2;
            value = p->category - 5;
            x = 0x60;
            y = 0x16;
            break;
        case 11:
        case 12:
            frame = 3;
            value = p->category - 11;
            x = 0x80;
            y = 0x16;
            break;
        case 13:
            frame = 4;
            value = -1;
            break;
        default:
            frame = 0;
            value = 1;
            break;
    }
    Anim_LoadKeyframe(&p->s18C, p->animData, (u16)(frame + 0x41), 2);
    Sprite_Show(&p->s18C);
    if (value >= 0) {
        Anim_LoadKeyframe(&p->s1EC, p->animData, (u16)(value + 1), 2);
        Sprite_Show(&p->s1EC);
        p->s1EC.position.x = x;
        p->s1EC.position.y = y;
    }
}
