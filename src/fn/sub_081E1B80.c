#include "global.h"
#define SoundTest_ShowCategory sub_081E1B80
// Provisional name: SoundTest_ShowCategory
struct Sprite {
    u8 head[8];
    u32 flags;
    u8 gap[0x20 - 12];
    s16 x, y;
    u8 tail[0x60 - 0x24];
};
struct Resource {
    u32 words[8];
};
struct Viewer {
    u8 p0[0x20];
    struct Sprite background;
    struct Resource backgroundResource;
    u8 p1[8];
    struct Sprite panel;
    struct Resource panelResource;
    u8 p2[8];
    struct Sprite cursor;
    struct Resource cursorResource;
    u8 p3[8];
    struct Sprite label;
    struct Resource labelResource;
    u8 p4[8];
    struct Resource digitsResource;
    struct Sprite digitsA[3], digitsB[3], labels[3];
    u8 p5[0x5cc - 0x5c0];
    u32 state, selection, countA, countB, countC, textIndex, valueA, valueB;
};
void sub_0821980C(void *, void *, u16, s32);
s32 Mod(s32, s32);
s32 Div(s32, s32);
static inline void show(struct Sprite *s)
{
    s->flags &= ~1;
}
// Show the selected category label and its cursor.
void SoundTest_ShowCategory(struct Viewer *p)
{
    show(&p->cursor);
    sub_0821980C(&p->label, &p->digitsResource, p->selection + 47, 0);
    p->label.flags &= ~1;
}
