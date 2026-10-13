#include "global.h"
struct Choice {
    u8 x, y, width, row;
};
struct Menu {
    u8 task[24];
    s8 selected, initial;
    u8 delay, count;
    u8 rowStart[8], rowEnd[8], rowCount[8];
    u8 rows, useScript, left, top;
    u8 pad38[2];
    u8 width, textX, allowCancel, wakeDelay, repeatDelay, explicitWidth;
    u32 held;
    u32 arguments[4];
    u32 script;
    void *callback, *window;
    u32 unknown60;
    struct Choice choices[16];
    u8 scratch[64];
    struct Resource {
        u32 words[8];
    } resource;
    u8 leftPrefix[8];
    u32 leftFlags;
    u8 pad110[20];
    s16 leftX, leftY;
    u8 pad128[68];
    u32 rightFlags;
    u8 pad170[20];
    s16 rightX, rightY;
};
struct Dimensions {
    u32 x : 16;
    u32 y : 16;
    u32 z : 16;
};
extern struct Menu *gUnk_0200002C;
extern u16 gUnk_03005260[];
struct Resource *sub_0821A520(s32, s32);
void sub_082196C4(struct Resource *, struct Resource *);
void sub_0821983C(void *, struct Resource *, s32, s32, s32, s32, s32, struct Dimensions *);
void sub_08219EBC(void *, void *, s32);
void ChoiceMenu_LayoutText(struct Menu *, void *);
void ChoiceMenu_Select(struct Menu *, s32);
void sub_08006738(struct Menu *);
s32 sub_08031E1C(void);
void sub_0822B2F8(s32);
s32 ChoiceMenu_Initialize(struct Menu *p, void *window, s32 stringBase, s32 *options)
{
    struct Resource *resource;
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
    p->unknown60 = stringBase;
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
    p->held = gUnk_03005260[0];
    p->left = options[3];
    p->top = options[4];
    p->pad38[0] = options[5];
    p->pad38[1] = options[6];
    p->delay = 2;
    dimensions.x = 1;
    dimensions.y = 0;
    dimensions.z = 0;
    sub_0821983C((u8 *)p + 0x104, &p->resource, 73, 17, 0, 0, 60, &dimensions);
    sub_0821983C((u8 *)p + 0x164, &p->resource, 74, 17, 0, 0, 60, &dimensions);
    palette = (u16 *)((u8 *)p + 0x1C8);
    sub_08219EBC(palette, *(void **)((u8 *)p + 0x14C), 32);
    *(u16 **)((u8 *)p + 0x14C) = palette;
    *(u16 **)((u8 *)p + 0x1AC) = palette;
    if (p->window) {
        ChoiceMenu_LayoutText(p, p->window);
        ChoiceMenu_Select(p, p->selected);
        sub_08006738(p);
    }
    if (!sub_08031E1C())
        sub_0822B2F8(263);
    return 0;
}
