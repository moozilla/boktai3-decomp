#include "global.h"

struct ResHdr {
    u32 words[8];
};
struct BookWidget {
    u8 pad0[8];
    u32 flags;
    u8 pad1[0x16];
    u16 x22;
    u8 pad2[0x24];
    void *pal;
    u8 pad3[0x14];
};
struct WidgetPos {
    s16 x, y, z, w;
};
struct MonsterBookGfx {
    u8 pad0[0x48];
    u8 page;
    u8 pad1[3];
    struct ResHdr hdr;
    struct BookWidget title;
    struct BookWidget w1;
    struct BookWidget w2;
    struct BookWidget w3;
    struct BookWidget w4;
    struct BookWidget w5;
    struct BookWidget rows[7];
    struct BookWidget cells[7][2];
    struct BookWidget a[2];
    struct BookWidget b;
    struct BookWidget c[2];
    struct BookWidget d;
    struct BookWidget e;
    u16 palette[16];
};

struct ResHdr *ScriptProc_Start(u32, u32);
void ResourceHeader_Relocate(struct ResHdr *, struct ResHdr *);
void Widget_InitLinked(struct BookWidget *, struct ResHdr *, u32, u32, u32, u32, u32,
                       struct WidgetPos *);

static inline void WidgetPos_Clear(struct WidgetPos *p)
{
    p->x = 0;
    p->y = 0;
    p->z = 0;
    p->w = 0;
}

u32 MonsterBook_InitWidgets(struct MonsterBookGfx *s)
{
    struct WidgetPos v;
    struct ResHdr *res;
    s32 i, j;
    struct BookWidget *d;

    res = ScriptProc_Start(0xCB05, 0xE2AB);
    s->hdr = *res;
    ResourceHeader_Relocate(&s->hdr, res);
    WidgetPos_Clear(&v);
    for (i = 0; i < 7; i++) {
        for (j = 0; j < 2; j++)
            Widget_InitLinked(&s->cells[i][j], &s->hdr, 0, 0x10, 0, 2, 0x3C, &v);
        Widget_InitLinked(&s->rows[i], &s->hdr, 0x14, 0x10, 0, 2, 0x3C, &v);
    }
    WidgetPos_Clear(&v);
    for (i = 0; i < 2; i++) {
        Widget_InitLinked(&s->a[i], &s->hdr, 0, 0x10, 0, 2, 0x3C, &v);
        Widget_InitLinked(&s->c[i], &s->hdr, 0, 0x10, 0, 2, 0x3C, &v);
    }
    Widget_InitLinked(&s->b, &s->hdr, 0x15, 0x10, 0, 2, 0x3C, &v);
    WidgetPos_Clear(&v);
    d = &s->d;
    Widget_InitLinked(d, &s->hdr, 0x3E, 0x10, 0, 2, 0x3C, &v);
    d->flags |= 1;
    WidgetPos_Clear(&v);
    Widget_InitLinked(&s->e, &s->hdr, 0x40, 0x10, 0, 2, 0x3C, &v);
    s->e.flags |= 1;
    WidgetPos_Clear(&v);
    Widget_InitLinked(&s->w5, &s->hdr, 0x3F, 0x10, 0, 2, 0x3C, &v);
    WidgetPos_Clear(&v);
    Widget_InitLinked(&s->w2, &s->hdr, 0x48, 0x10, 0, 2, 0x3C, &v);
    WidgetPos_Clear(&v);
    Widget_InitLinked(&s->w4, &s->hdr, 1, 0x10, 0, 2, 0x3C, &v);
    WidgetPos_Clear(&v);
    Widget_InitLinked(&s->w3, &s->hdr, 0x41, 0x10, 0, 2, 0x3C, &v);
    WidgetPos_Clear(&v);
    Widget_InitLinked(&s->title, &s->hdr, 0x46, 0x10, 0, 2, 0x3C, &v);
    s->title.x22 = s->page << 4;
    WidgetPos_Clear(&v);
    Widget_InitLinked(&s->w1, &s->hdr, 0x47, 0x10, 0, 2, 0x3C, &v);
    CpuSet(s->title.pal, s->palette, 0x04000008);
    s->title.pal = s->palette;
    s->w1.pal = s->palette;
}
