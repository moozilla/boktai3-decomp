#include "global.h"
#include "region_menu.h"
#include "menu_parameters.h"
#include "runtime.h"
#include "render.h"
#include "input.h"

void Menu_EraseRect(s32, s32, s32, s32);

void Menu_DrawStringRef(s32, s32, u32, s32);

void *Task_Create(u32, u32);
void Task_SetCallbacks(void *, void (*)(), void (*)());
void Task_Kill(void *);
void Anim_Remove(void *);
void *ScriptProc_Start(u32, u32);
void ResourceHeader_Relocate(void *, void *);
void Bg_ClearTilemap(u32);
void Script_Call(u32, void *);
u8 *Script_GetPc(void);
s32 Script_GetValue(void);
s32 Script_ReadValue();
s32 Script_SeekToKeyword(s32);

void Time_ShowRegionMenu(struct RegionMenu *menu)
{
    s32 first = 1;
    s32 row = 0;
    s32 i;
    menu->skipped = row;
    Menu_EraseRect(4, 7, 0x14, 10);
    for (i = 0; i < menu->count; i++) {
        if (menu->entries[i].group == menu->page) {
            if (first == 1)
                first = 0;
            Menu_DrawStringRef(4, row + 7, gRegionMenuParameters.nBlock, i);
            row += 2;
        }
        menu->skipped += first;
    }
}

struct TimeState {
    u8 pad0[0x18];
    u32 region;
};

extern u8 gSunClock[];

void Time_CalculateSunriseSunset(s32 latitude, s32 longitude, s32 timezone);

void Save_WriteHeader(void);

void Time_OnRegionListSelect(struct RegionMenu *menu)
{
    Time_CalculateSunriseSunset(menu->entries[menu->selected].latitude,
                                menu->entries[menu->selected].longitude, 0);
    gSunClock[9] = 0;
    {
        s32 sel = menu->selected;
        ((struct TimeState *)gSessionState)->region = sel;
    }
    Save_WriteHeader();
}

struct Rect16 {
    u32 x : 16;
    u32 y : 16;
    u32 z : 16;
    u32 w : 16;
};

struct ScriptCallArgs {
    u32 count : 16;
    u32 unused : 16;
    s32 *values;
};

void Sound_PlaySong(u32);

void Time_OnRegionListSelect(struct RegionMenu *);

void Time_ShowRegionMenu(struct RegionMenu *);

void Menu_StartFadeOut(s32);

s32 Anim_LoadKeyframe(void *, void *, u32, u32);

void Menu_SetTextRect(void *, struct Rect16 *);

void Palette_UpdateHighlightPulse(void *, u8 *);

void Time_HandleRegionKeypad(struct RegionMenu *menu)
{
    s32 value = 1;
    struct ScriptCallArgs args;
    struct Rect16 rect;
    args.count = 1;
    args.values = &value;
    value = 1;
    switch (menu->state) {
        case 0:
            if (gPadState[1] & 1) {
                Sound_PlaySong(0xdd);
                if (gRegionMenuParameters.flag == 1)
                    menu->fadeSpeed = 3;
                else
                    menu->fadeSpeed = 0;
                Time_OnRegionListSelect(menu);
                Menu_StartFadeOut(menu->fadeSpeed);
                menu->state = 1;
                menu->confirmed = 1;
            } else if (gPadState[1] & 2) {
                Sound_PlaySong(0xde);
                Menu_StartFadeOut(menu->fadeSpeed);
                menu->state = 1;
                menu->confirmed = 0;
            } else {
                u8 old;
                if (gPadState[1] & 0x100) {
                    if (++menu->page > 13)
                        menu->page = 1;
                    Sound_PlaySong(0xdc);
                    menu->selected = menu->pageFirst[menu->page];
                    Anim_LoadKeyframe(&menu->anims[1], menu->anims[1].resource,
                                      (u16)(menu->page + 0x18), 0);
                    Time_ShowRegionMenu(menu);
                }
                if (gPadState[1] & 0x200) {
                    if (--menu->page <= 0)
                        menu->page = 13;
                    Sound_PlaySong(0xdc);
                    menu->selected = menu->pageFirst[menu->page];
                    Anim_LoadKeyframe(&menu->anims[1], menu->anims[1].resource,
                                      (u16)(menu->page + 0x18), 0);
                    Time_ShowRegionMenu(menu);
                }
                if (gPadState[1] & 0x40) {
                    old = menu->selected;
                    menu->selected = old - 1;
                    if (menu->entries[menu->selected].group != menu->page)
                        menu->selected = old;
                    else
                        Sound_PlaySong(0xdc);
                }
                if (gPadState[1] & 0x80) {
                    old = menu->selected;
                    menu->selected = old + 1;
                    if (menu->entries[menu->selected].group != menu->page)
                        menu->selected = old;
                    else
                        Sound_PlaySong(0xdc);
                }
                rect.x = 0;
                rect.y = (menu->selected - menu->skipped) * 16 + 0x38;
                rect.z = 0;
                Menu_SetTextRect(&menu->anims[3], &rect);
                menu->anims[3].flags &= ~1;
                Palette_UpdateHighlightPulse(menu->highlightPalette.pulse.pulseState,
                                             &menu->highlightPulse);
            }
            break;
        case 1:
            if (menu->timer > (1 << menu->fadeSpeed)) {
                {
                    s16 *p = &gDispCntShadow;
                    s32 m = ~3;
                    *p &= m;
                }
                menu->anims[3].flags |= 1;
                menu->anims[2].flags |= 1;
                menu->anims[1].flags |= 1;
                menu->anims[0].flags |= 1;
                if (menu->confirmed == 0)
                    Script_Call(gRegionMenuParameters.cancelProc, &args);
                else
                    Script_Call(gRegionMenuParameters.confirmProc, &args);
                Task_Kill(menu);
            }
            menu->timer++;
            break;
        case 2:
            break;
    }
}

void Time_DestroyRegionMenu(struct RegionMenu *menu)
{
    Anim_Remove(&menu->anims[3]);
    Anim_Remove(&menu->anims[2]);
    Anim_Remove(&menu->anims[1]);
    Anim_Remove(&menu->anims[0]);
}

void BgGfx_LoadTilemapSimple(u32, u32, u32, u32, u32);

void Bg_SetupLayers(u32, u32, void *, u32, u32, u32, u32 *);

void Widget_InitLinked(void *, void *, u32, u32, u32, u32, u32, u32);

void Menu_StartFadeIn(u32);

u8 *Script_SetPc(u8 *pc);

u8 *Script_GetPc(void);

s32 Script_GetValue(void);


s32 Time_LoadRegionList(struct RegionMenu *menu)
{
    s32 n, k, count, group;
    u32 zero;
    void *res;
    void *tiles;
    u32 value;
    struct Rect16 rect;
    if (gRegionMenuParameters.flag == 1)
        menu->selected = 6;
    else
        menu->selected = ((struct TimeState *)gSessionState)->region;
    if (menu->selected == 0)
        menu->selected = 1;
    menu->fadeSpeed = zero = 0;
    Bg_ClearTilemap(0);
    BgGfx_LoadTilemapSimple(0, 0, 0, 0, zero);
    Bg_ClearTilemap(2);
    BgGfx_LoadTilemapSimple(2, 0, 0, 0, zero);
    Bg_ClearTilemap(3);
    tiles = ScriptProc_Start(0xC091, 0x3536);
    value = 3;
    Bg_SetupLayers(2, 0, tiles, 0, zero, 1, &value);
    BgGfx_LoadTilemapSimple(3, 0, 0, 0, zero);
    CpuSet((u8 *)ScriptProc_Start(0x92B3, 0x204) + 0x14, gBgPaletteBuffer, 0x100);
    Script_SetPc((u8 *)gRegionMenuParameters.table);
    n = 0;
    while (n <= 64 && Script_GetPc()) {
        group = Script_GetValue();
        if (group == -1) {
            menu->count = n;
            break;
        }
        menu->pageFirst[group] = n;
        count = Script_GetValue();
        for (k = 0; k < count && Script_GetPc(); n++, k++) {
            menu->entries[n].group = group;
            menu->entries[n].name = Script_ReadValue();
            menu->entries[n].longitude = Script_GetValue();
            menu->entries[n].latitude = Script_GetValue();
        }
    }
    menu->page = menu->entries[menu->selected].group;
    res = ScriptProc_Start(0xCB05, 0xE2AB);
    ResourceHeader_Relocate(menu->anims[0].resource, res);
    Widget_InitLinked(&menu->anims[0], menu->anims[0].resource, 0x35, 0x10, 0, 0, 0, 0);
    ResourceHeader_Relocate(menu->anims[1].resource, res);
    Widget_InitLinked(&menu->anims[1], menu->anims[1].resource, (u16)(menu->page + 0x18), 0x10, 0,
                      0, 0, 0);
    res = ScriptProc_Start(0xCB05, 0x530D);
    ResourceHeader_Relocate(menu->anims[2].resource, res);
    Widget_InitLinked(&menu->anims[2], menu->anims[2].resource, 0x8a, 0x10, 0, 0, 0, 0);
    ResourceHeader_Relocate(menu->anims[3].resource, res);
    Widget_InitLinked(&menu->anims[3], menu->anims[3].resource, 0x48, 0x10, 0, 0, 0, 0);
    menu->anims[3].flags |= 1;
    Time_ShowRegionMenu(menu);
    {
        void **saved = &menu->savedPalette;
        void **pal = &menu->anims[3].palette;
        u16 *copy;
        *saved = *pal;
        copy = menu->highlightPalette.colors;
        CpuSet(*saved, copy, 0x10);
        *pal = copy;
    }
    Time_ShowRegionMenu(menu);
    rect.x = 0;
    rect.y = (menu->selected - menu->skipped) * 16 + 0x38;
    rect.z = 0;
    Menu_SetTextRect(&menu->anims[3], &rect);
    menu->anims[3].flags &= ~1;
    Menu_StartFadeIn(menu->fadeSpeed);
    return 0;
}

s32 Script_GetValueSafe(void);

/* Parameters at 0x02000310 (meaning unknown). */

/* Script: -n block, -d (read with Script_GetValueSafe), -o, -c and -f
   (default 0). */

void ScreenParams310_SetFromScript(void)
{
    struct RegionMenuParameters *p = &gRegionMenuParameters;
    p->flag = 0;
    if (Script_SeekToKeyword('n'))
        p->nBlock = Script_GetPc();
    if (Script_SeekToKeyword('d'))
        p->table = Script_GetValueSafe();
    if (Script_SeekToKeyword('o'))
        p->confirmProc = Script_GetValue();
    if (Script_SeekToKeyword('c'))
        p->cancelProc = Script_GetValue();
    if (Script_SeekToKeyword('f'))
        p->flag = Script_GetValue();
}

void *Time_CreateRegionMenu(void)
{
    void *p = Task_Create(0xb, 0x5b8);
    if (p) {
        Task_SetCallbacks(p, Time_HandleRegionKeypad, Time_DestroyRegionMenu);
        if (Time_LoadRegionList(p) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}

/* Registry routine 7978: -d points at a region table (groups of entries:
   group id or -1 to end, count, then per entry a name string, longitude and
   latitude; at most 64 entries). Computes sunrise and sunset for the saved
   region (1-53, else 6). */

void Time_OnLoadSave(void)
{
    struct RegionEntry entries[64];
    s32 n, k, count, group;
    u32 region;
    u8 *pc;
    if (!Script_SeekToKeyword('d'))
        return;
    Script_SetPc((u8 *)Script_GetValueSafe());
    n = 0;
    while (n <= 64 && Script_GetPc() && (group = Script_GetValue()) != -1) {
        count = Script_GetValue();
        for (k = 0; k < count && (pc = Script_GetPc()); n++, k++) {
            entries[n].group = group;
            entries[n].name = Script_ReadValue(pc);
            entries[n].longitude = Script_GetValue();
            entries[n].latitude = Script_GetValue();
        }
    }
    region = ((struct TimeState *)gSessionState)->region;
    if (region - 1 > 52) {
        ((struct TimeState *)gSessionState)->region = 6;
        region = 6;
    }
    Time_CalculateSunriseSunset(entries[region].latitude, entries[region].longitude, 0);
    gSunClock[9] = 0;
}
