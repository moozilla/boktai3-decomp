#include "global.h"
#include "engine.h"

struct PanelSprite {
    u8 unk0[8];
    u32 flags;
    u8 unkC[0x20 - 0xC];
    s16 x, y;
};
struct SelectionPanel {
    u8 unk0[0x64];
    u8 keyframes[0x1600 - 0x64];
    struct PanelSprite cursor;
    u8 unk1624[0x26E0 - 0x1624];
    u8 firstValue[0x2740 - 0x26E0];
    u8 secondValue[0x27A0 - 0x2740];
    u8 thirdValue[0x3760 - 0x27A0];
    struct PanelSprite leftArrow;
    u8 unk3784[0x37C0 - 0x3784];
    struct PanelSprite rightArrow;
    u8 unk37E4[0x48C0 - 0x37E4];
    u8 row, first, second, third;
};
static inline void hide(struct PanelSprite *s)
{
    s->flags |= 1;
}
static inline void show(struct PanelSprite *s)
{
    s->flags &= ~1;
}
// confidence=low: hides (visible == 0) or places the row cursor and arrows, then refreshes the
// three value digits.
#define SelectionPanel_UpdateCursor sub_0817597C
void SelectionPanel_UpdateCursor(struct SelectionPanel *p, s32 visible)
{
    if (visible == 0) {
        hide(&p->cursor);
        hide(&p->leftArrow);
        hide(&p->rightArrow);
    } else {
        u8 *row;
        p->cursor.flags &= ~1;
        p->cursor.x = 0;
        row = &p->row;
        p->cursor.y = *row * 16 + 0x18;
        show(&p->leftArrow);
        show(&p->rightArrow);
        p->leftArrow.x = 0x80;
        p->rightArrow.x = 0xd8;
        {
            s32 y = *row * 16 + 0x18;
            p->leftArrow.y = y;
            p->rightArrow.y = y;
        }
    }
    Anim_LoadKeyframe(p->firstValue, p->keyframes, p->first + 0x4d, 1);
    Anim_LoadKeyframe(p->secondValue, p->keyframes, p->second + 0x4d, 1);
    {
        s8 first = 0x4e;
        Anim_LoadKeyframe(p->thirdValue, p->keyframes, (u16)(first - p->third), 1);
    }
}
