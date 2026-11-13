#include "date_entry_menu.h"
#include "sun_clock.h"
#include "menu_parameters.h"
#include "input.h"
#include "render.h"
#include "global.h"

void DateMenu_HideWidgets(struct DateMenu *p)
{
    u32 one = 1;
    u32 *flags = &p->digits[0].flags;
    s32 i = 11;
    do {
        *flags |= one;
        flags = (u32 *)((u8 *)flags + sizeof(struct SceneInputWidget));
    } while (--i >= 0);
    p->widgets[3].sprite.flags |= 1;
    p->widgets[2].sprite.flags |= 1;
    p->widgets[1].sprite.flags |= 1;
    p->widgets[0].sprite.flags |= 1;
}

void DateMenu_InitializeDigits(struct DateMenu *menu)
{
    struct ContactPosition pos[12];
    s32 i;
    memcpy(pos, gUnk_0824FCCC, sizeof(pos));
    for (i = 0; i < 12; i++) {
        Widget_InitLinked((u8 *)&menu->digits[i], &menu->widgets[0].resource, 0, 0x30, 0, 0, 0,
                          &pos[i]);
        menu->digits[i].flags |= 1;
    }
    menu->year = 0;
    menu->month = 0;
    menu->day = 0;
    menu->hour = 0;
    menu->minute = 0;
}

void DateMenu_LoadCurrentDate(struct DateMenu *p)
{
    s32 v, t;
    s32 i;
    u32 *flags;
    v = SunClock_GetPackedDate() >> 16;
    p->year = ((u32)v >> 12) * 1000 + ((u32)(v & 0xf00) >> 8) * 100 + ((u32)(v & 0xf0) >> 4) * 10 +
              (v & 0xf);
    t = SunClock_GetPackedDate() & 0xff00;
    v = t >> 8;
    p->month = ((u32)v >> 4) * 10 + (v & 0xf);
    v = SunClock_GetPackedDate() & 0xff;
    p->day = ((u32)v >> 4) * 10 + (v & 0xf);
    p->hour = SunClock_GetBaseHour();
    p->minute = SunClock_GetBaseMinute();
    t = ~1;
    flags = &p->digits[0].flags;
    for (i = 11; i >= 0; i--) {
        *flags &= t;
        flags = (u32 *)((u8 *)flags + sizeof(struct SceneInputWidget));
    }
}

void DateMenu_UpdateCursor(struct DateMenu *p)
{
    struct ContactPosition r;
    p->widgets[1].sprite.flags |= 1;
    p->widgets[2].sprite.flags &= ~1;
    switch (*(u32 *)&p->selection) {
        case 0:
            *(u32 *)&r = 0;
            p->widgets[1].sprite.flags &= ~1;
            p->widgets[2].sprite.flags |= 1;
            Menu_SetTextRect(&p->widgets[1].sprite, &r);
            break;
        case 1:
            r.x = 0;
            r.y = 0;
            Menu_SetTextRect(&p->widgets[2].sprite, &r);
            break;
        case 2:
            r.x = 0x30;
            r.y = 0;
            Menu_SetTextRect(&p->widgets[2].sprite, &r);
            break;
        case 3:
            r.x = -0x20;
            r.y = 0x20;
            Menu_SetTextRect(&p->widgets[2].sprite, &r);
            break;
        case 4:
            r.x = 0x10;
            r.y = 0x20;
            Menu_SetTextRect(&p->widgets[2].sprite, &r);
            break;
    }
}

void DateMenu_UpdateDigits(struct DateMenu *p)
{
    s32 v, i;
    struct SceneInputWidget *digits;

    if (p->year != p->oldYear) {
        digits = &p->digits[0];
        v = p->year;
        for (i = 0; i <= 3; i++) {
            Anim_LoadKeyframe(&digits[3 - i], &p->digitResource, (u16)Mod(v, 10), 0);
            v = Div(v, 10);
        }
    }
    if (p->month != p->oldMonth) {
        digits = &p->digits[4];
        v = p->month;
        for (i = 0; i <= 1; i++) {
            Anim_LoadKeyframe(&digits[1 - i], &p->digitResource, (u16)Mod(v, 10), 0);
            v = Div(v, 10);
        }
    }
    if (p->year != p->oldYear || p->month != p->oldMonth) {
        v = Clock_DaysInMonth(p->year, p->month);
        if (p->day > v)
            p->day = v;
    }
    if (p->day != p->oldDay) {
        digits = &p->digits[6];
        v = p->day;
        for (i = 0; i <= 1; i++) {
            Anim_LoadKeyframe(&digits[1 - i], &p->digitResource, (u16)Mod(v, 10), 0);
            v = Div(v, 10);
        }
    }
    if (p->hour != p->oldHour) {
        digits = &p->digits[8];
        v = p->hour;
        for (i = 0; i <= 1; i++) {
            Anim_LoadKeyframe(&digits[1 - i], &p->digitResource, (u16)Mod(v, 10), 0);
            v = Div(v, 10);
        }
    }
    if (p->minute != p->oldMinute) {
        digits = &p->digits[10];
        v = p->minute;
        for (i = 0; i <= 1; i++) {
            Anim_LoadKeyframe(&digits[1 - i], &p->digitResource, (u16)Mod(v, 10), 0);
            v = Div(v, 10);
        }
    }
}

enum { CLOCK_DIR_NONE, CLOCK_DIR_UP, CLOCK_DIR_DOWN };

void DateMenu_HandleInput(struct DateMenu *p)
{
    s32 dir = CLOCK_DIR_NONE;
    s32 days;

    if (gPadState[1] & 0x40) {
        dir = CLOCK_DIR_UP;
    } else if (gPadState[1] & 0x80) {
        dir = CLOCK_DIR_DOWN;
    } else if (gPadState[0] & 0xC0) {
        if (++p->repeatTimer > 14 && Mod(p->repeatTimer, 5) == 0) {
            if (gPadState[0] & 0x40)
                dir = CLOCK_DIR_UP;
            else
                dir = CLOCK_DIR_DOWN;
        }
    } else {
        p->repeatTimer = dir;
    }

    switch (p->selection) {
        case 0:
            if (dir == CLOCK_DIR_UP) {
                if (p->year <= 2098) {
                    p->year++;
                    Sound_PlaySong(0x19A);
                }
            } else if (dir == CLOCK_DIR_DOWN) {
                if (p->year > 2000) {
                    p->year--;
                    Sound_PlaySong(0x19A);
                }
            }
            break;
        case 1:
            if (dir == CLOCK_DIR_UP) {
                if (p->month > 11)
                    p->month = 1;
                else
                    p->month++;
                Sound_PlaySong(0x19A);
            } else if (dir == CLOCK_DIR_DOWN) {
                if (p->month <= 1)
                    p->month = 12;
                else
                    p->month--;
                Sound_PlaySong(0x19A);
            }
            break;
        case 2:
            days = Clock_DaysInMonth(p->year, p->month);
            if (dir == CLOCK_DIR_UP) {
                if (p->day >= days)
                    p->day = 1;
                else
                    p->day++;
                Sound_PlaySong(0x19A);
            } else if (dir == CLOCK_DIR_DOWN) {
                if (p->day <= 1)
                    p->day = days;
                else
                    p->day--;
                Sound_PlaySong(0x19A);
            }
            break;
        case 3:
            if (dir == CLOCK_DIR_UP) {
                if (p->hour > 22)
                    p->hour = 0;
                else
                    p->hour++;
                Sound_PlaySong(0x19A);
            } else if (dir == CLOCK_DIR_DOWN) {
                if (p->hour <= 0)
                    p->hour = 23;
                else
                    p->hour--;
                Sound_PlaySong(0x19A);
            }
            break;
        case 4:
            if (dir == CLOCK_DIR_UP) {
                if (p->minute > 58)
                    p->minute = 0;
                else
                    p->minute++;
                Sound_PlaySong(0x19A);
            } else if (dir == CLOCK_DIR_DOWN) {
                if (p->minute <= 0)
                    p->minute = 59;
                else
                    p->minute--;
                Sound_PlaySong(0x19A);
            }
            break;
    }
    DateMenu_UpdateDigits(p);
}

void DateMenu_Update(struct DateMenu *p)
{
    switch (p->state) {
        case 2:
            break;
        case 0:
            p->oldYear = p->year;
            p->oldMonth = p->month;
            p->oldDay = p->day;
            p->oldHour = p->hour;
            p->oldMinute = p->minute;
            if (gPadState[1] & 0x20) {
                if (p->selection <= 0)
                    p->selection = 4;
                else
                    p->selection--;
                Sound_PlaySong(0xDC);
            } else if (gPadState[1] & 0x10) {
                if (p->selection > 3)
                    p->selection = 0;
                else
                    p->selection++;
                Sound_PlaySong(0xDC);
            }
            DateMenu_HandleInput(p);
            if (gPadState[1] & 8) {
                Sound_PlaySong(0xDD);
                SunClock_SetDateTime(p->year, p->month, p->day, p->hour, p->minute, 0, 1);
                Time_CalculateSunriseSunset(0x23A666, 0x8BC000, 0);
                gSunClock.pad9 = 0;
                Menu_StartFadeOut(p->fadeShift);
                p->state = 1;
                p->result = 1;
            }
            DateMenu_UpdateCursor(p);
            Palette_UpdateHighlightPulse(&p->colors[12], p->pulseState);
            break;
        case 1:
            if (p->timer > (1 << p->fadeShift)) {
                DateMenu_HideWidgets(p);
                if (p->result == 0)
                    Script_Call(gDateMenuParameters.cancelProc, 0);
                else
                    Script_Call(gDateMenuParameters.confirmProc, 0);
                Task_Kill(p);
            }
            p->timer++;
            break;
    }
}

void DateMenu_Destroy(struct DateMenu *p)
{
    struct SceneInputWidget *entry = p->digits;
    s32 i = 11;
    do {
        Anim_Remove(entry);
        entry++;
    } while (--i >= 0);
    Anim_Remove(&p->widgets[3].sprite);
    Anim_Remove(&p->widgets[2].sprite);
    Anim_Remove(&p->widgets[1].sprite);
    Anim_Remove(&p->widgets[0].sprite);
}

static inline void RelocateDateCursor(struct DateMenu *menu, void *source,
                                      struct SceneInputResource **graphics)
{
    *graphics = &menu->widgets[3].resource;
    ResourceHeader_Relocate(*graphics, source);
}

s32 DateMenu_Initialize(struct DateMenu *menu)
{
    u32 zero;
    u32 one;
    u32 resourceBank;
    void *resource;
    struct SceneInputResource *firstGraphics;
    struct SceneInputResource *secondGraphics;
    struct SceneInputResource *cursorGraphics;
    u32 layer;
    void *bg;
    u16 **firstPalette;
    u16 *colors;
    u16 firstPaletteBank;
    u16 *paletteBank;
    menu->state = zero = 0;
    menu->selection = zero;
    Bg_ClearTilemap(0);
    BgGfx_LoadTilemapSimple(0, 0, 0, 0, zero);
    Bg_ClearTilemap(2);
    BgGfx_LoadTilemapSimple(2, 0, 0, 0, zero);
    Bg_ClearTilemap(3);
    BgGfx_LoadTilemapSimple(3, 0, 0, 0, zero);
    bg = ScriptProc_Start(0xC091, 0x3536);
    layer = 2;
    Bg_SetupLayers(2, 0, bg, 0, zero, one = 1, &layer);
    BgGfx_LoadTilemapSimple(3, 0, 0, 0, zero);
    CpuSet(((struct DateMenuPalette *)ScriptProc_Start(0x92B3, 0x204))->colors, gBgPaletteBuffer,
           0x100);

    resourceBank = 0xCB05;
    resource = ScriptProc_Start(resourceBank, 0xE2AB);
    firstGraphics = &menu->widgets[0].resource;
    ResourceHeader_Relocate(firstGraphics, resource);
    menu->digitResource = *firstGraphics;
    Widget_InitLinked((u8 *)&menu->widgets[0], firstGraphics, 0x34, 0x10, zero, zero, zero, NULL);
    secondGraphics = &menu->widgets[1].resource;
    ResourceHeader_Relocate(secondGraphics, resource);
    Widget_InitLinked((u8 *)&menu->widgets[1], secondGraphics, 0x26, 0x10, zero, zero, zero, NULL);

    ResourceHeader_Relocate(&menu->widgets[2].resource, resource);
    Widget_InitLinked((u8 *)&menu->widgets[2], &menu->widgets[2].resource, 0x27, 0x10, zero, zero,
                      zero, NULL);
    menu->widgets[1].sprite.flags |= one;
    menu->widgets[2].sprite.flags |= one;
    {
        u16 **savedPalette = &menu->savedPalette;
        u16 *sourcePalette;
        firstPalette = &menu->widgets[1].sprite.palette;
        sourcePalette = *firstPalette;
        *savedPalette = sourcePalette;
        colors = menu->colors;
        CpuSet(sourcePalette, colors, 0x10);
        *firstPalette = colors;
    }
    menu->widgets[2].sprite.palette = colors;
    paletteBank = &menu->widgets[1].sprite.paletteBank;
    firstPaletteBank = 0xE000;
    *paletteBank = firstPaletteBank;
    paletteBank = &menu->widgets[2].sprite.paletteBank;
    *paletteBank = firstPaletteBank + 1;
    resource = ScriptProc_Start(resourceBank, 0x530D);
    RelocateDateCursor(menu, resource, &cursorGraphics);
    Widget_InitLinked((u8 *)&menu->widgets[3], cursorGraphics, 0x80, 0x10, zero, zero, zero, NULL);
    DateMenu_InitializeDigits(menu);
    DateMenu_LoadCurrentDate(menu);
    DateMenu_UpdateDigits(menu);
    if (gDateMenuParameters.flag == 1) {
        menu->fadeShift = zero;
    } else {
        menu->fadeShift = 3;
    }
    Menu_StartFadeIn(menu->fadeShift);
    return 0;
}

void DateMenu_SetParametersFromScript(void)
{
    if (Script_SeekToKeyword(0x6f))
        gDateMenuParameters.confirmProc = Script_GetValue();
    if (Script_SeekToKeyword(0x63))
        gDateMenuParameters.cancelProc = Script_GetValue();
    if (Script_SeekToKeyword(0x66))
        gDateMenuParameters.flag = Script_GetValue();
}

void *DateMenu_Create(void)
{
    struct DateMenu *r = Task_Create(0xb, sizeof(struct DateMenu));
    if (r != 0) {
        Task_SetCallbacks(r, DateMenu_Update, DateMenu_Destroy);
        if (DateMenu_Initialize(r) < 0) {
            Task_Kill(r);
            return 0;
        }
    }
    return r;
}
