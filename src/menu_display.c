#include "menu_display.h"

void SolarCalibration_Update(struct SolarCalibrationMenu *s)
{
    u32 result;
    struct ScriptUtilityArguments args;

    result = 0;
    args.count = 1;
    args.values = &result;
    switch (s->state) {
        case 0:
            if (Mod(s->blinkTimer, 30) == 0) {
                s32 off = s->page * sizeof(struct SolarCalibrationWidget);
                u8 *base = (u8 *)&s->widgets[1].sprite.flags;
                u32 *flags = (u32 *)(base + off);
                u32 v = *flags;
                if (v & 1)
                    v &= ~1;
                else
                    v |= 1;
                *flags = v;
            }
            if (Mod(s->pageTimer, 180) == 0) {
                if (s->page == 0) {
                    Menu_SetBackgroundMap(s->backgroundMap, 7);
                    s->page = 1 - s->page;
                    s->widgets[1].sprite.flags |= 1;
                    s->widgets[2].sprite.flags &= ~1;
                } else {
                    Menu_SetBackgroundMap(s->backgroundMap, 6);
                    s->page = 1 - s->page;
                    s->widgets[1].sprite.flags &= ~1;
                    s->widgets[2].sprite.flags |= 1;
                }
            }
            s->pageTimer++;
            s->blinkTimer++;
            if ((gPadState[1] & 1) && s->timer > 30) {
                s32 off = s->page * sizeof(struct SolarCalibrationWidget);
                u8 *base = (u8 *)&s->widgets[1].sprite.flags;
                *(u32 *)(base + off) &= ~1;
                s->timer = 0;
                s->state = 1;
            } else if (gPadState[1] & 2) {
                if (gMessageMenuParameters.f == 0 && gMessageMenuParameters.c != 0) {
                    Sound_PlaySong(0xDE);
                    s->state = 5;
                    s->cancelState = 0;
                }
            }
            break;
        case 1:
            if (s->timer <= 59) {
                if (Solar_CaptureBaseline() != 0) {
                    if (gMessageMenuParameters.f == 0)
                        Save_WriteHeader();
                    s->timer = 0;
                    s->state = 2;
                }
            } else {
                s->timer = 0;
                s->state = 3;
            }
            break;
        case 2:
            Bg_ClearTilemap(0);
            BgGfx_LoadTilemapSimple(0, 0, 0, 0, 0);
            MessageWindow_Clear();
            MessageWindow_ShowMessage(1);
            MessageWindow_SetBounds(0x28, 5, 0x18, 2);
            Sound_PlaySong(0xDD);
            s->timer = 0;
            s->state = 4;
            break;
        case 3:
            Bg_ClearTilemap(0);
            BgGfx_LoadTilemapSimple(0, 0, 0, 0, 0);
            MessageWindow_Clear();
            MessageWindow_ShowMessage(2);
            MessageWindow_SetBounds(0x26, 5, 0x18, 2);
            Sound_PlaySong(0x192);
            s->timer = 0;
            s->state = 4;
            break;
        case 4:
            if (s->timer > 59) {
                Menu_StartFadeOut(s->fadeSpeed);
                s->timer = 0;
                s->state = 5;
            }
            break;
        case 5:
            if (s->fadeTimer > (1 << s->fadeSpeed)) {
                Bg_ClearTilemap(0);
                BgGfx_LoadTilemapSimple(0, 0, 0, 0, 0);
                MessageWindow_Clear();
                s->widgets[1].sprite.flags |= 1;
                s->widgets[2].sprite.flags |= 1;
                Script_Call(gMessageMenuParameters.o, &args);
                Task_Kill(s);
            }
            s->fadeTimer++;
            break;
        case 6:
            break;
    }
    s->timer++;
}

void SolarCalibration_Destroy(struct SolarCalibrationMenu *p)
{
    u16 *r = &gDispCntShadow;
    u32 m = ~3;
    *r = m & *r;
    p->widgets[0].sprite.flags |= 1;
    Anim_Remove(&p->widgets[2].sprite);
    Anim_Remove(&p->widgets[1].sprite);
    Anim_Remove(&p->widgets[0].sprite);
}

s32 SolarCalibration_Initialize(struct SolarCalibrationMenu *s)
{
    u32 zero, one, value;
    void *a, *p, *resource;
    struct SceneInputResource *obj, *obj2, *obj3;
    struct MessageMenuParameters *global;
    Bg_ClearTilemap(0);
    zero = 0;
    BgGfx_LoadTilemapSimple(0, 0, 0, 0, zero);
    Bg_ClearTilemap(2);
    BgGfx_LoadTilemapSimple(2, 0, 0, 0, zero);
    Bg_ClearTilemap(3);
    BgGfx_LoadTilemapSimple(3, 0, 0, 0, zero);
    a = ScriptProc_Start(0xC091, 0x3536);
    s->backgroundMap = a;
    value = 6;
    Bg_SetupLayers(2, 0, a, 0, zero, one = 1, &value);
    BgGfx_LoadTilemapSimple(3, 0, 0, 0, zero);
    p = ((struct MenuDisplayPalette *)ScriptProc_Start(0x92B3, 0x204))->colors;
    CpuSet(p, gBgPaletteBuffer, 0x100);
    resource = ScriptProc_Start(0xCB05, 0xE2AB);
    obj = &s->widgets[0].graphics;
    ResourceHeader_Relocate(obj, resource);
    Widget_InitLinked(&s->widgets[0].sprite, obj, 0x38, 0x10, zero, zero, zero, zero);
    obj2 = &s->widgets[1].graphics;
    ResourceHeader_Relocate(obj2, resource);
    Widget_InitLinked(&s->widgets[1].sprite, obj2, 0x29, 0x10, zero, zero, zero, zero);
    obj3 = &s->widgets[2].graphics;
    ResourceHeader_Relocate(obj3, resource);
    Widget_InitLinked(&s->widgets[2].sprite, obj3, 0x2A, 0x10, zero, zero, zero, zero);
    s->widgets[1].sprite.flags &= ~1;
    s->widgets[2].sprite.flags |= one;
    MessageWindow_SetPalette(0xC486);
    global = &gMessageMenuParameters;
    MessageWindow_SetMessageTable(global->lBlock);
    MessageWindow_ShowMessage(0);
    MessageWindow_SetBounds(2, 3, 0x20, 6);
    MessageWindow_SetDisplayMode(1);
    s->pageTimer = one;
    s->blinkTimer = one;
    if (global->f == 1)
        s->fadeSpeed = 3;
    else
        s->fadeSpeed = zero;
    Menu_StartFadeIn(s->fadeSpeed);
    return s->fadeSpeed = 0;
}

void SolarCalibration_SetParametersFromScript(void)
{
    struct MessageMenuParameters *p = &gMessageMenuParameters;
    p->c = 0;
    p->f = 0;
    if (Script_SeekToKeyword('l'))
        p->lBlock = Script_GetPc();
    if (Script_SeekToKeyword('o'))
        p->o = Script_GetValue();
    if (Script_SeekToKeyword('c'))
        p->c = Script_GetValue();
    if (Script_SeekToKeyword('f'))
        p->f = Script_GetValue();
}

struct SolarCalibrationMenu *SolarCalibration_Create(void)
{
    struct SolarCalibrationMenu *r = Task_Create(0xb, sizeof(*r));
    if (r != 0) {
        Task_SetCallbacks(r, (void (*)(void *))SolarCalibration_Update,
                          (void (*)(void *))SolarCalibration_Destroy);
        if (SolarCalibration_Initialize(r) < 0) {
            Task_Kill(r);
            return 0;
        }
    }
    return r;
}

void Menu_EraseRect(s32 a, s32 b, s32 c, s32 d)
{
    TextWindow_BlankRect(a, b, c, d);
    BackgroundMap_ClearTextRect(a, b, c, d);
}

void Menu_DrawText(s32 left, s32 row, const u8 *s)
{
    s32 x = left;
    s32 y = row;
    u32 highlight = 0;
    u32 glyph;
    u32 c;
    for (;;) {
        c = *s;
        c <<= 24;
        if (!c)
            return;
        if ((s32)c >= 0) {
            if (*s == '<') {
                highlight = 1;
                s = Text_FindChar(s, '>');
            } else if (*s == '\n') {
                x = left;
                y += 2;
            } else if (*s == 0x1f) {
                u32 code = (u16)Text_DecodeGlyphEscape(s);
                if (code == 0xFFFF) {
                    glyph = (u16)(*s - 0x20);
                    TextWindow_DrawNarrowGlyph(glyph, x, y, highlight);
                    x++;
                } else if (code & 0xFF00) {
                    u32 mask = 0x7FFF;
                    code &= mask;
                    TextWindow_DrawWideGlyph(code, x, y, highlight);
                    x += 2;
                } else {
                    TextWindow_DrawNarrowGlyph(code, x, y, highlight);
                    x++;
                }
                s++;
            } else {
                glyph = (u16)(*s - 0x20);
                TextWindow_DrawNarrowGlyph(glyph, x, y, highlight);
                x++;
            }
        } else {
            u32 low = *s;
            u32 mask = 0x7f;
            c = mask & low;
            s++;
            low = *s;
            glyph = (c << 8) | low;
            if (glyph >= Text_GetWideGlyphCount())
                x += 2;
            else {
                TextWindow_DrawWideGlyph(glyph, x, y, highlight);
                x += 2;
                if (highlight == 1) {
                    s++;
                    s = Text_FindChar(s, '>');
                    highlight = 0;
                }
            }
        }
        s++;
    }
}

void Menu_DrawStringRef(s32 column, s32 row, const void *source, s32 index)
{
    Menu_DrawText(column, row, Text_LookupString(Script_ParseStringRef(source) + index));
}

void Menu_SetTextRect(struct SceneInputWidget *widget, const struct ContactPosition *position)
{
    widget->position.x = position->x;
    widget->position.y = position->y;
    widget->position.z = position->z;
    widget->position.pad = position->pad;
}

void Menu_SetBackgroundMap(void *p, u32 b)
{
    Bg_SetLayerMap(3, p, b);
    BgGfx_LoadTilemapSimple(3, 0, 0, 0, 0);
}

void Menu_StartFadeIn(u32 a)
{
    if (a != 0) {
        ScreenTransition_SendCommand(0, a, 4, 4, 4, 0xFFFF, 0);
    }
}

void Menu_StartFadeOut(u32 a)
{
    if (a != 0) {
        ScreenTransition_SendCommand(1, a, 4, 4, 4, 0xFFFF, 0);
    }
}
