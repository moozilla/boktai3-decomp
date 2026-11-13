#include "field_hud.h"

s32 Div(s32, s32);
void CpuSet(const void *, void *, u32);
void PaletteFade_SetScrollPosition(s32, s32);
struct SceneHudState *Hud_GetManager(void);
void Hud_SetValueSource(void *, u32);

static inline s32 FieldHud_GetLocalPlayer(void)
{
    if (!gLinkSession)
        return -1;
    return gLinkSession->localPlayer;
}

void FieldHud_OnUnusedUpdate(void)
{
}

void FieldHud_OnUnusedCleanup(void)
{
}

u16 *Background_GetWrappedTileEntry(u32 i, u32 x, u32 y)
{
    struct BackgroundLayer *layer = &gBgRecords[i];
    u8 *base = (u8 *)layer->map;

    return (u16 *)(base + ((x & 0x1F) << 1) + ((y & 0x1F) << 6));
}

void FieldHud_UpdateGaugePalette(struct FieldHudWidgets *widgets)
{
    u16 palette[16];
    s32 level, i;
    if (widgets->gaugeSource == NULL)
        level = 0;
    else
        level = Div(*widgets->gaugeSource, 0x898);
    if (widgets->fill > level)
        level = widgets->fill - 1;
    else if (widgets->fill < level)
        level = widgets->fill + 1;
    palette[0] = widgets->paletteResource[0xE0];
    palette[14] = widgets->paletteResource[0xEE];
    palette[15] = widgets->paletteResource[0xEF];
    for (i = 0; i <= 12; i++) {
        if (i < level)
            palette[i + 1] = widgets->paletteResource[0xE1 + i];
        else
            palette[i + 1] = widgets->paletteResource[0xEE];
    }
    CpuSet(palette, gBgPaletteBank14, 0x10);
    widgets->fill = level;
}

void FieldHud_RefreshGaugeIndex(struct FieldHudWidgets *widgets)
{
    s32 index = widgets->fill * 2 + 1;
    if (widgets->displayedIndex != index)
        PaletteFade_SetScrollPosition(index, 0);
}

void FieldHud_UpdateTimeDigits(struct FieldHudWidgets *widgets)
{
    s32 **source = &widgets->timeSource;
    s32 minutes, fraction, digit;
    if (!*source)
        return;
    minutes = Div(**source, 60);
    fraction = Div((**source - minutes * 60) * 169, 10);
    if (**source > 59999) {
        Anim_SelectWidgetGlyph(&widgets->timeDigits[0], widgets->timeResource, 50);
        Anim_SelectWidgetGlyph(&widgets->timeDigits[1], widgets->timeResource, 50);
        Anim_SelectWidgetGlyph(&widgets->timeDigits[2], widgets->timeResource, 50);
        Anim_SelectWidgetGlyph(&widgets->timeDigits[3], widgets->timeResource, 50);
        Anim_SelectWidgetGlyph(&widgets->timeDigits[4], widgets->timeResource, 50);
        Anim_SelectWidgetGlyph(&widgets->timeDigits[5], widgets->timeResource, 50);
    } else {
        digit = 0;
        while (minutes > 99) {
            digit++;
            minutes -= 100;
        }
        Anim_SelectWidgetGlyph(&widgets->timeDigits[0], widgets->timeResource, (u16)(digit + 41));
        digit = 0;
        while (minutes > 9) {
            digit++;
            minutes -= 10;
        }
        Anim_SelectWidgetGlyph(&widgets->timeDigits[1], widgets->timeResource, (u16)(digit + 41));
        Anim_SelectWidgetGlyph(&widgets->timeDigits[2], widgets->timeResource, (u16)(minutes + 41));
        digit = 0;
        while (fraction > 99) {
            digit++;
            fraction -= 100;
        }
        Anim_SelectWidgetGlyph(&widgets->timeDigits[3], widgets->timeResource, (u16)(digit + 41));
        digit = 0;
        while (fraction > 9) {
            digit++;
            fraction -= 10;
        }
        Anim_SelectWidgetGlyph(&widgets->timeDigits[4], widgets->timeResource, (u16)(digit + 41));
        Anim_SelectWidgetGlyph(&widgets->timeDigits[5], widgets->timeResource,
                               (u16)(fraction + 41));
    }
}

s32 NumericDisplay_InitializeWidgets(struct FieldHudWidgets *widgets, s32 *timeSource)
{
    s32 i;
    s32 x;
    s32 result, ret;
    void *script;
    script = ScriptProc_Start(0xCB05, 0x530D);
    widgets->timeScript = script;
    ResourceHeader_Relocate(widgets->timeResource, script);
    i = 0;
    do {
        switch (i) {
            case 0:
                x = 0x58;
                break;
            case 1:
                x = 0x60;
                break;
            case 2:
                x = 0x68;
                break;
            case 3:
                x = 0x78;
                break;
            case 4:
                x = 0x80;
                break;
            case 5:
                x = 0x88;
                break;
            default:
                x = 0;
                break;
        }
        result = Widget_InitLinked(&widgets->timeDigits[i], widgets->timeResource, 0x29, 0x10, 0, 0,
                                   0, 0);
        if (result != 0) {
            ret = -1;
            goto end;
        }
        widgets->timeDigits[i].position.x = x;
        widgets->timeDigits[i].position.z = result;
        widgets->timeDigits[i].position.y = result;
        widgets->timeDigits[i].flags &= ~1;
        widgets->timeDigits[i].palette = gPaletteBankData;
    } while (++i <= 5);
    widgets->timeSource = timeSource;
    ret = 0;
end:
    return ret;
}

void Palette_InitColorTransition(struct FieldHudWidgets *widgets, u32 x, u32 y, u32 z, u32 endX,
                                 u32 endY, u32 endZ, u32 duration)
{
    widgets->colorStart = x | (y << 5) | (z << 10);
    widgets->numberColor = endX | (endY << 5) | (endZ << 10);
    widgets->timer = duration;
    widgets->deltaR = Div((endX - x) << 4, widgets->timer);
    widgets->deltaG = Div((endY - y) << 4, widgets->timer);
    widgets->deltaB = Div((endZ - z) << 4, widgets->timer);
    widgets->red = x << 4;
    widgets->green = y << 4;
    widgets->blue = z << 4;
}

void FieldHud_UpdateNumberDisplay(struct FieldHudWidgets *widgets)
{
    s32 value, digit;
    u16 *tile;
    if (widgets->value != *widgets->source) {
        if (widgets->value > *widgets->source) {
            if (widgets->value - *widgets->source > 50)
                widgets->value -= 10;
            else
                widgets->value--;
        } else {
            if (*widgets->source - widgets->value > 50)
                widgets->value += 10;
            else
                widgets->value++;
        }
    }
    value = widgets->value;
    if (value > 999)
        value = 999;
    if (value < 0)
        value = 0;
    widgets->numberResource[7] =
        (widgets->red >> 4) | ((widgets->green >> 4) << 5) | ((widgets->blue >> 4) << 10);
    CpuSet(widgets->numberResource, gBgPaletteBank13, 8);
    if (value <= widgets->numberStep) {
        widgets->red += widgets->deltaR;
        widgets->green += widgets->deltaG;
        widgets->blue += widgets->deltaB;
        if (widgets->timer == 0) {
            switch (widgets->colorPhase) {
                case 0:
                    Palette_InitColorTransition(widgets, 4, 4, 4, 31, 0, 0, 20);
                    widgets->colorPhase = 1;
                    break;
                case 1:
                    Palette_InitColorTransition(widgets, 31, 0, 0, 31, 31, 31, 10);
                    widgets->colorPhase = 2;
                    break;
                case 2:
                    Palette_InitColorTransition(widgets, 31, 31, 31, 31, 0, 0, 10);
                    widgets->colorPhase = 3;
                    break;
                case 3:
                    Palette_InitColorTransition(widgets, 31, 0, 0, 4, 4, 4, 20);
                    widgets->colorPhase = 0;
                    break;
            }
        } else
            widgets->timer--;
    } else {
        widgets->red = 64;
        widgets->green = 64;
        widgets->blue = 64;
        widgets->colorPhase = 0;
        widgets->timer = 0;
    }
    tile = Background_GetWrappedTileEntry(0, 3, 1);
    digit = 0;
    while (value > 99) {
        digit++;
        value -= 100;
    }
    *tile = (u16)((digit + 17) | 0xD000);
    tile = Background_GetWrappedTileEntry(0, 4, 1);
    digit = 0;
    while (value > 9) {
        digit++;
        value -= 10;
    }
    *tile = (u16)((digit + 17) | 0xD000);
    tile = Background_GetWrappedTileEntry(0, 5, 1);
    *tile = (u16)((value + 17) | 0xD000);
    tile = Background_GetWrappedTileEntry(0, 6, 1);
    *tile = 0xd01c;
    value = widgets->displayedValue;
    tile = Background_GetWrappedTileEntry(0, 7, 1);
    digit = 0;
    while (value > 99) {
        digit++;
        value -= 100;
    }
    *tile = (u16)((digit + 17) | 0xD000);
    tile = Background_GetWrappedTileEntry(0, 8, 1);
    digit = 0;
    while (value > 9) {
        digit++;
        value -= 10;
    }
    *tile = (u16)((digit + 17) | 0xD000);
    tile = Background_GetWrappedTileEntry(0, 9, 1);
    *tile = (u16)((value + 17) | 0xD000);
}

s32 ViewportGauge_ComputeFill(struct FieldHudWidgets *widgets, s32 slot)
{
    struct ScenePoolPlayer *subject;
    s32 value;
    if (slot > 1 || !(subject = gParticipantActors[slot]))
        return 0;
    if (!widgets->meterScale)
        value = 0;
    else
        value = Div((subject->position[0] - subject->viewportOriginX) >> 12, widgets->meterScale);
    return Div(value * 72, 100);
}

void MeterDisplay_InitializeWidgets(struct FieldHudWidgets *widgets)
{
    s32 i, result;
    u32 zero;
    u32 *flags;
    struct SceneInputWidget *widget;
    void *script;
    script = ScriptProc_Start(0xCB05, 0x1289);
    widgets->meterScript = script;
    ResourceHeader_Relocate(widgets->meterResource, script);
    i = 0;
    zero = 0;
    widget = widgets->meterWidgets;

    do {
        result = Widget_InitLinked(widget, widgets->meterResource, (u16)(i + 0x49), 0x10, zero,
                                   zero, zero, (void *)zero);
        if (result)
            return;
        widget->position.x = result;
        widget->position.y = result;
        widget->position.z = result;
        flags = &widgets->meterWidgets[0].flags;
        flags = (u32 *)((u8 *)flags + i * sizeof(*widget));
        *flags &= ~1;
        widget->attribute1A = zero;
        widget++;

    } while (++i <= 1);
    /* The background API still declares this shared pointer with an opaque Camera tag. */
    widgets->meterScale = Div(((struct SceneArcCamera *)gSceneCamera)->viewportWidth << 5, 100);
}

void FieldHud_InitializeNumber(struct FieldHudWidgets *widgets, s32 *source)
{
    s32 value;
    widgets->source = source;
    widgets->value = *source;
    value = widgets->value;
    if (widgets->value > 999)
        value = 999;
    widgets->displayedValue = value;
    widgets->numberColor = 0x1084;
    widgets->numberStep = Div(widgets->value * 3, 10);
    CpuSet((u8 *)ScriptProc_Start(0x92B3, 0x6458) + 0x1B4, widgets->numberResource, 8);
}

void FieldHud_InitializeGauge(struct FieldHudWidgets *widgets, s32 *source)
{
    widgets->fill = 0;
    widgets->paletteResource = (const u16 *)((u8 *)ScriptProc_Start(0x92B3, 0x6458) + 0x14);
    widgets->gaugeSource = source;
}

void MeterDisplay_UpdateOwnerMeters(struct FieldHudWidgets *widgets)
{
    u32 mask = 0x800;
    if ((gEngineFlags | gScenePauseFlags) & mask) {
        widgets->meterWidgets[0].position.x =
            ViewportGauge_ComputeFill(widgets, FieldHud_GetLocalPlayer());
        widgets->meterWidgets[1].position.x =
            ViewportGauge_ComputeFill(widgets, FieldHud_GetLocalPlayer() ^ 1);
    } else {
        widgets->meterWidgets[0].position.x = ViewportGauge_ComputeFill(widgets, 0);
        widgets->meterWidgets[1].flags |= 1;
    }
}

void FieldHud_UpdateWidgets(struct SceneHudState *hud)
{
    struct FieldHudWidgets *widgets = &hud->fieldWidgets;
    FieldHud_UpdateNumberDisplay(widgets);
    FieldHud_UpdateGaugePalette(widgets);
    FieldHud_RefreshGaugeIndex(widgets);
    MeterDisplay_UpdateOwnerMeters(widgets);
    FieldHud_UpdateTimeDigits(widgets);
}

void FieldHud_InitializeWidgets(s32 *value, s32 *gauge, void *valueSource, u16 valueMode,
                                s32 *timeSource)
{
    struct SceneHudState *owner = Hud_GetManager();
    struct FieldHudWidgets *widgets = &owner->fieldWidgets;
    FieldHud_InitializeNumber(widgets, value);
    FieldHud_InitializeGauge(widgets, gauge);
    Hud_SetValueSource(valueSource, valueMode);
    MeterDisplay_InitializeWidgets(widgets);
    NumericDisplay_InitializeWidgets(widgets, timeSource);
    owner->initializationFlag = 1;
}
