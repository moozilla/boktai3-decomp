#include "scanline_presentation.h"
#include "render.h"
#include "engine.h"
#include "ram.h"

// MATCHING_CONCESSION: register-pinning

static inline int ScanlinePresentation_NextNoiseRandom(void)
{
    const u16 *table = gRandomTable;
    gRandomIndex = (gRandomIndex + 1) & 1023;
    return table[gRandomIndex];
}

void ScanlinePresentation_UpdateNoise(struct ScanlinePresentation *p)
{
    s32 i;
    s16 value;
    register s16 *ramp asm("r1");
    s16 *amplitude, *delta;
    u16 *end;
    register u32 amplitudeAddress;
    register u32 endOffset asm("r8");
    i = 0;
    if (i < p->start) {
        u16 *active = &p->active;
        u16(*rows)[160] = p->rows;
        register u32 zero asm("ip") = 0;
        do {
            u32 columnOffset = i * sizeof(u16);
            u32 bufferOffset = *active * sizeof(p->rows[0]);
            *(u16 *)((u8 *)rows + (columnOffset + bufferOffset)) = zero;
            i++;
        } while (i < p->start);
    }
    i = p->start;
    endOffset = (u8 *)&p->end - (u8 *)p;
    end = (u16 *)((u8 *)p + endOffset);
    amplitudeAddress = (u8 *)&p->amplitude - (u8 *)p;
    amplitudeAddress += (u32)p;
    amplitude = (s16 *)amplitudeAddress;
    delta = &p->delta;
    for (; i < *end; i++, end = (u16 *)((u8 *)p + endOffset)) {
        s32 v;
        v = (ScanlinePresentation_NextNoiseRandom() * p->amplitude) >> 18;
        p->rows[p->active][i] = v | (v << 8);
    }
    for (i = p->end; i < 160; i++)
        p->rows[p->active][i] = 0;
    ramp = amplitude;
    value = (*ramp += *delta);
    if (value < 0)
        *ramp = 0;
    else if (value > 128)
        *amplitude = 128;
}

s32 Div(s32, s32);

void ScanlinePresentation_UpdateFadeIn(struct ScanlinePresentation *s)
{
    if (s->mode == 0)
        gBgFadeLevel = Div(s->phase << 6, s->duration);
    s->phase++;
    if (s->phase > s->duration) {
        s->phase = s->duration;
        s->fade = 0;
    }
}

void ScanlinePresentation_UpdateFadeOut(struct ScanlinePresentation *s)
{
    u32 k;
    if (s->mode == 0)
        gBgFadeLevel = Div(s->phase << 6, s->duration);
    s->phase--;
    k = 0;
    if (s->phase < 0) {
        s->phase = k;
        s->fade = (void *)k;
        if (s->script != 0)
            Script_Call(s->script, 0);
        Task_Kill(s);
    }
}

void ScanlineEffect_SetSource(int, int, void *);

static inline void hide(struct ScanlineLayer *p)
{
    p->flags |= 1;
}

int ScanlinePresentation_Update(struct ScanlinePresentation *p)
{
    if (p->mode == 0)
        gObjFadeLevel = gBgFadeLevel;
    if (p->cancellable && (gPadState[1] & 3)) {
        hide(&p->layers[0]);
        hide(&p->layers[1]);
        hide(&p->layers[2]);
        hide(&p->layers[3]);
        hide(&p->layers[4]);
        hide(&p->layers[5]);
        if (p->mode == 0)
            gObjFadeLevel = gBgFadeLevel = 0;
        if (p->script)
            Script_Call(p->script, 0);
        Task_Kill(p);
    } else {
        if (p->fade)
            p->fade(p);
        if (p->noise) {
            p->noise(p);
            ScanlineEffect_SetSource(0, 2, p->rows[p->active]);
            p->active = 1 - p->active;
        }
        switch (p->state) {
            case 0:
                p->timer++;
                if (p->timer > p->delay) {
                    p->state = 1;
                    p->timer = 0;
                    p->fade = ScanlinePresentation_UpdateFadeIn;
                    if (p->start != p->end)
                        Sound_PlaySong(225);
                    p->amplitude = 128;
                    p->delta = -2;
                    p->noise = ScanlinePresentation_UpdateNoise;
                }
                break;
            case 1:
                if (!p->fade)
                    p->state = 2;
                break;
            case 2:
                p->timer++;
                if (p->timer > p->hold) {
                    p->state = 3;
                    p->timer = 0;
                    p->fade = ScanlinePresentation_UpdateFadeOut;
                    if (p->start != p->end)
                        Sound_PlaySong(227);
                    p->amplitude = 0;
                    p->delta = 4;
                    p->noise = ScanlinePresentation_UpdateNoise;
                }
                break;
        }
        p->ticks++;
    }
    return 0;
}

void sub_082194D4(void *);

void Mosaic_ResetParameters(void);

int ScanlinePresentation_Destroy(struct ScanlinePresentation *p)
{
    int i;
    struct ScanlineLayer *layer;
    sub_082194D4(p);
    Mosaic_ResetParameters();
    Anim_Remove(&p->layers[0]);
    Anim_Remove(&p->layers[1]);
    Anim_Remove(&p->layers[2]);
    Anim_Remove(&p->layers[3]);
    layer = &p->layers[4];
    for (i = 1; i >= 0; i--) {
        Anim_Remove(layer);
        layer++;
    }
    return 0;
}

void Widget_InitLinked(void *, void *, u32, u32, u32, u32, u32, void *);
s32 ResourceHeader_Relocate(void *, void *);
u32 Script_GetTextSourcePc(void);
s32 Mod(s32, s32);
s32 MessageWindow_SetMessageTable(u32);
s32 MessageWindow_SetBounds(s32, s32, s32, s32);
s32 MessageWindow_SetDisplayMode(s32);
s32 MessageWindow_ShowMessage(s32);
void MessageWindow_SetPalette(u32);
s32 MessageWindow_GetRemainingTextWidth(void);
void Mosaic_SetParameters(s32, s32, s32);

struct WidgetAttr {
    u32 a;
    u16 b, c;
};

s32 sub_0800DC90(struct ScanlinePresentation *p)
{
    struct WidgetAttr attr;
    s32 digits[2];
    s32 message, shown, number, *d;
    struct ScanlineLayer *layer;
    s32 i;
    u32 handle;
    s32 x;
    void *res;
    s16 *mode;
    s8 *pb;
    u32 v;
    u16 *q;
    s32 w;

    gBgPaletteBuffer[0] = 0x1084;
    attr.b = 0;
    attr.a = 0;
    p->fade = 0;
    p->phase = 0;
    p->state = 0;
    p->cancellable = Script_GetKeywordValue(0x43, 0);
    p->mode = Script_GetKeywordValue(0x6E, 0);
    if (p->mode == 0)
        p->delay = 0x40;
    else
        p->delay = 0;
    p->duration = 0x40;
    p->hold = Script_GetKeywordValue(0x64, 0x80);
    message = Script_GetKeywordValue(0x69, 0);
    shown = Script_GetKeywordValue(0x63, 0);
    number = Script_GetKeywordValue(0x73, -1);
    p->script = Script_GetKeywordValue(0x65, 0);
    p->digitsA = Script_GetKeywordValue(0x6F, -1);
    p->digitsB = Script_GetKeywordValue(0x74, -1);
    p->banner = Script_GetKeywordValue(0x6C, 0);
    handle = Script_SeekToKeyword(0x72);
    if (handle)
        handle = Script_GetTextSourcePc();
    p->handle = handle;
    x = Script_SeekToKeyword(0x79);
    if (x) {
        p->start = Script_GetValue();
        p->end = Script_GetValue();
    } else {
        p->start = x;
        p->end = x;
    }
    res = ScriptProc_Start(0xCB05, 0xE89F);
    if (res == 0)
        return -1;
    p->header = *(struct ScanlineResourceHeader *)res;
    ResourceHeader_Relocate(&p->header, res);
    if (number >= 0)
        Widget_InitLinked(&p->layers[0], &p->header, (u16)(number + 0x17), 0x870, 0, 0, 0x3C,
                          &attr);
    else
        Widget_InitLinked(&p->layers[0], &p->header, 0x17, 0x871, 0, 0, 0x3C, &attr);
    Widget_InitLinked(&p->layers[1], &p->header, 0x16, 0x871, 0, 0, 0x3C, &attr);
    if (shown)
        p->layers[1].flags &= ~1;
    Widget_InitLinked(&p->layers[3], &p->header, p->banner ? 0x26 : 1, 0x871, 0, 0, 0x3C, &attr);
    if (p->digitsA >= 0) {
        p->layers[3].flags &= ~1;
        digits[0] = Mod(p->digitsA, 10);
        digits[1] = Div(p->digitsA, 10);
        digits[0] += 2;
        digits[1] += 12;
    } else {
        if (p->banner)
            p->layers[3].flags &= ~1;
        digits[0] = 2;
        digits[1] = 12;
    }
    mode = &p->mode;
    d = digits;
    layer = &p->layers[4];
    for (i = 0; i < 2; i++) {
        Widget_InitLinked(layer, &p->header, (u16)*d, 0x871, 0, 0, 0x3C, &attr);
        if (p->digitsA >= 0) {
            s32 off = i * 0x60;
            u32 *f = &p->layers[4].flags;
            *(u32 *)((u8 *)f + off) &= ~1;
        }
        d++;
        layer++;
    }
    pb = &p->digitsB;
    v = *pb < 0 ? 0 : *pb;
    Widget_InitLinked(&p->layers[2], &p->header, (u16)v, 0x871, 0, 0, 0x3C, &attr);
    if (*pb >= 0)
        p->layers[2].flags &= ~1;
    if (p->handle) {
        MessageWindow_SetDisplayMode(1);
        MessageWindow_SetPalette(0x539C);
        MessageWindow_SetMessageTable(p->handle);
        MessageWindow_ShowMessage(message);
        MessageWindow_SetBounds(0, 7, 0x1A, 2);
        w = MessageWindow_GetRemainingTextWidth();
        q = (u16 *)gBgRecords;
        q[16] = w * 4 - 0x78;
        Mosaic_SetParameters(0, 0, 0x11);
    } else {
        Mosaic_SetParameters(0, 0, 0x10);
    }
    if (*mode == 0) {
        gBgFadeLevel = 0;
        gBgFadeColor = 0x1084;
        gBgFadeBankMask = 0xFFFF;
        gObjFadeLevel = 0;
        gObjFadeColor = 0x1084;
        gObjFadeFlags = 0;
    }
    return 0;
}

void *ScanlinePresentation_Create(void)
{
    struct ScanlinePresentation *p = Task_Create(8, 0x52C);
    if (p) {
        Task_SetCallbacks(p, (void *)ScanlinePresentation_Update,
                          (void *)ScanlinePresentation_Destroy);
        if (sub_0800DC90(p) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
