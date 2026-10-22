#include "global.h"
#ifdef NONMATCHING
// fuzzy: 87.0%
struct ResultDrawing {
    u8 unk0[8];
    u32 flags;
    u8 unkC[0x54];
};
struct ResultItem {
    struct ResultDrawing frame;
    u8 digits[0x60];
};
struct WirelessResultPanel {
    u8 task[0x18];
    u32 timer;
    u8 state, entered;
    u8 time0[3];
    u8 time1[3];
    u8 side;
    u8 pad25;
    u8 mode;
    u8 pad27;
    s16 score0, score1;
    u8 pad2c[4];
    u8 resource[0x38];
    struct ResultDrawing cursor;
    u8 padC8[0x50];
    struct ResultItem items[9];
    u8 pad7d8[0x120];
    u32 script;
    void *textScript;
    int windows[4];
    u8 string0[0x10];
    u8 string1[0x10];
};
#define PANEL_U32(p, off) (*(u32 *)((u8 *)(p) + (off)))
extern u16 gUnk_03004BD8;
void *WirelessResultPanel_GetInstance(void);
void Hud_SetMode(int);
void Bg_ClearTilemap(int);
void WirelessResultPanel_InitBackground(void *, int);
void Video_SetRenderCallbacks(int, void *, void *, void *);
void SpriteOam_QueueOverlayProjectedSprites(void);
void Sprite_RenderUnflippedList(void);
void sub_0821969C(void);
int Div(int, int);
int Mod(int, int);
void Anim_DrawDecimalDigits(void *, void *, int, int, int, int, int, int, int);
int TextWindow_Create(int, int, int, int);
void TextWindow_SetScript(int, void *);
void TextWindow_SetString(int, int, void *);
void TextWindow_Refresh(int, int);
void TextWindow_StartPlayback(int);

static inline s32 ClampScore(s32 v)
{
    if (v > 9999)
        v = 9999;
    else if (v < 0)
        v = 0;
    return v;
}

static inline void ResultItem_Hide(struct ResultItem *item)
{
    item->frame.flags &= ~1;
}

#define ResultItem_DrawDigits(anim, resource, value, digits, x, y)                                 \
    do {                                                                                           \
        void *a_ = (anim);                                                                         \
        Anim_DrawDecimalDigits(a_, resource, value, digits, 0x33, x, y, 8, 0);                     \
    } while (0)

int sub_0803B700(int mode, int side, int time0, int time1, int score0, int score1, u32 script)
{
    struct WirelessResultPanel *p = WirelessResultPanel_GetInstance();
    u32 zero;
    int rem;
    if (p == NULL)
        return -1;
    p->mode = mode;
    zero = 0;
    p->side = side;
    p->script = script;
    gUnk_03004BD8 = 0xF1FF & gUnk_03004BD8;
    Hud_SetMode(7);
    Bg_ClearTilemap(0);
    WirelessResultPanel_InitBackground(p, 1);
    Video_SetRenderCallbacks(0, SpriteOam_QueueOverlayProjectedSprites, Sprite_RenderUnflippedList,
                             sub_0821969C);
    p->state = 1;
    p->entered = 1;
    p->timer = zero;
    p->time0[0] = Div(time0, 3600);
    rem = Mod(time0, 3600);
    p->time0[1] = Div(rem, 60);
    p->time0[2] = Div(Mod(rem, 60) * 100, 60);
    p->time1[0] = Div(time1, 3600);
    rem = Mod(time1, 3600);
    p->time1[1] = Div(rem, 60);
    p->time1[2] = Div(Mod(rem, 60) * 100, 60);
    p->score0 = score0;
    p->score1 = score1;
    if ((s16)score0 > 9999)
        p->score0 = 9999;
    else if ((s16)score0 < 0)
        p->score0 = zero;
    if (p->score1 > 9999)
        p->score1 = 9999;
    else if (p->score1 < 0)
        p->score1 = 0;
    ResultItem_Hide(&p->items[0]);
    ResultItem_DrawDigits(p->items[0].digits, p->resource, 0, 2, 0xa8, 0x20);
    ResultItem_DrawDigits(p->items[1].digits, p->resource, 0, 2, 0xc0, 0x20);
    ResultItem_DrawDigits(p->items[2].digits, p->resource, 0, 2, 0xd8, 0x20);
    ResultItem_DrawDigits(p->items[3].digits, p->resource, 0, 2, 0xa8, 0x38);
    ResultItem_DrawDigits(p->items[4].digits, p->resource, 0, 2, 0xc0, 0x38);
    ResultItem_DrawDigits(p->items[5].digits, p->resource, 0, 2, 0xd8, 0x38);
    ResultItem_DrawDigits(p->items[6].digits, p->resource, 0, 4, 0x68, 0x60);
    ResultItem_DrawDigits(p->items[8].digits, p->resource, ClampScore(p->score1), 4, 0x68, 0x88);
    p->windows[0] = TextWindow_Create(10, 4, 10, 2);
    if (p->windows[0] < 0)
        return -1;
    TextWindow_SetScript(p->windows[0], p->textScript);
    TextWindow_SetString(p->windows[0], 0, p->string0);
    TextWindow_Refresh(p->windows[0], 3);
    TextWindow_StartPlayback(p->windows[0]);
    p->windows[1] = TextWindow_Create(10, 7, 10, 2);
    if (p->windows[1] < 0)
        return -1;
    TextWindow_SetScript(p->windows[1], p->textScript);
    TextWindow_SetString(p->windows[1], 1, p->string1);
    TextWindow_Refresh(p->windows[1], 4);
    TextWindow_StartPlayback(p->windows[1]);
    p->windows[2] = TextWindow_Create(1, 10, 0x1c, 2);
    if (p->windows[2] < 0)
        return -1;
    TextWindow_SetScript(p->windows[2], p->textScript);
    TextWindow_Refresh(p->windows[2], 0xb);
    TextWindow_StartPlayback(p->windows[2]);
    p->windows[3] = TextWindow_Create(1, 0xf, 0x1c, 2);
    if (p->windows[3] < 0)
        return -1;
    TextWindow_SetScript(p->windows[3], p->textScript);
    TextWindow_Refresh(p->windows[3], 0xc);
    TextWindow_StartPlayback(p->windows[3]);
    p->cursor.flags &= ~1;
    *(u16 *)((u8 *)p + 0x8c) = 0x18;
    *(u16 *)((u8 *)p + 0x8e) = 0x20;
    return 0;
}
#else
INCLUDE_ASM("asm/nonmatching", sub_0803B700);
#endif
