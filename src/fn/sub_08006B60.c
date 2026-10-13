#include "choice_menu.h"
struct Dimensions {
    u32 x : 16;
    u32 y : 16;
    u32 z : 16;
};
struct ChoiceResource *sub_0821A520(s32, s32);
void sub_082196C4(struct ChoiceResource *, struct ChoiceResource *);
void sub_0821983C(void *, struct ChoiceResource *, s32, s32, s32, s32, s32, struct Dimensions *);
void sub_08219EBC(void *, void *, s32);
s32 sub_08031E1C(void);
void sub_0822B2F8(s32);
s32 ChoiceMenu_Initialize(struct ChoiceMenu *p, void *window, s32 stringBase, s32 *options)
{
    struct ChoiceResource *resource;
    struct Dimensions dimensions;
    s32 initial;
    u16 *palette;
    gUnk_0200002C = p;
    resource = sub_0821A520(0xCB05, 0x530D);
    if (!resource)
        return -1;
    p->resource = *resource;
    sub_082196C4(&p->resource, resource);
    p->window = window;
    p->stringBase = stringBase;
    if (options[1]) {
        p->selected = -1;
        initial = options[0];
    } else {
        initial = options[0];
        p->selected = initial;
    }
    if (initial < 0)
        initial = 0;
    p->initial = initial;
    p->allowCancel = options[2];
    p->wakeDelay = 10;
    p->repeatDelay = 0;
    p->held = gUnk_03005260.held;
    p->left = options[3];
    p->top = options[4];
    p->maxWidth = options[5];
    p->maxRows = options[6];
    p->delay = 2;
    dimensions.x = 1;
    dimensions.y = 0;
    dimensions.z = 0;
    sub_0821983C(&p->leftCursor, &p->resource, 73, 17, 0, 0, 60, &dimensions);
    sub_0821983C(&p->rightCursor, &p->resource, 74, 17, 0, 0, 60, &dimensions);
    palette = p->palette;
    sub_08219EBC(palette, p->leftCursor.palette, 32);
    p->leftCursor.palette = palette;
    p->rightCursor.palette = palette;
    if (p->window) {
        ChoiceMenu_LayoutText(p, p->window);
        ChoiceMenu_Select(p, p->selected);
        sub_08006738(p);
    }
    if (!sub_08031E1C())
        sub_0822B2F8(263);
    return 0;
}
