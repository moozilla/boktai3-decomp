#include "region_menu.h"

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

void Time_OnRegionListSelect(struct RegionMenu *menu)
{
    Time_CalculateSunriseSunset(menu->entries[menu->selected].latitude,
                                menu->entries[menu->selected].longitude, 0);
    gSunClock.pad9 = 0;
    {
        s32 sel = menu->selected;
        ((struct SessionSave *)gSessionState)->region = sel;
    }
    Save_WriteHeader();
}

void Time_HandleRegionKeypad(struct RegionMenu *menu)
{
    s32 value = 1;
    struct ScriptUtilityArguments args;
    struct SceneInputPackedPosition rect;
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
                    Anim_LoadKeyframe(&menu->widgets[1].sprite, &menu->widgets[1].resource,
                                      (u16)(menu->page + 0x18), 0);
                    Time_ShowRegionMenu(menu);
                }
                if (gPadState[1] & 0x200) {
                    if (--menu->page <= 0)
                        menu->page = 13;
                    Sound_PlaySong(0xdc);
                    menu->selected = menu->pageFirst[menu->page];
                    Anim_LoadKeyframe(&menu->widgets[1].sprite, &menu->widgets[1].resource,
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
                Menu_SetTextRect(&menu->widgets[3].sprite, &rect);
                menu->widgets[3].sprite.flags &= ~1;
                Palette_UpdateHighlightPulse(menu->palette + 12, &menu->pulse);
            }
            break;
        case 1:
            if (menu->timer > (1 << menu->fadeSpeed)) {
                {
                    s16 *p = &gDispCntShadow;
                    s32 m = ~3;
                    *p &= m;
                }
                menu->widgets[3].sprite.flags |= 1;
                menu->widgets[2].sprite.flags |= 1;
                menu->widgets[1].sprite.flags |= 1;
                menu->widgets[0].sprite.flags |= 1;
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

void Time_DestroyRegionMenu(struct RegionMenu *p)
{
    Anim_Remove(&p->widgets[3].sprite);
    Anim_Remove(&p->widgets[2].sprite);
    Anim_Remove(&p->widgets[1].sprite);
    Anim_Remove(&p->widgets[0].sprite);
}

s32 Time_LoadRegionList(struct RegionMenu *menu)
{
    s32 n, k, count, group;
    u8 *pc;
    u32 zero;
    void *res;
    void *tiles;
    u32 value;
    struct SceneInputPackedPosition rect;
    if (gRegionMenuParameters.flag == 1)
        menu->selected = 6;
    else
        menu->selected = ((struct SessionSave *)gSessionState)->region;
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
    CpuSet(((struct RegionMenuPalette *)ScriptProc_Start(0x92B3, 0x204))->colors, gBgPaletteBuffer,
           0x100);
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
        for (k = 0; k < count && (pc = Script_GetPc()); n++, k++) {
            menu->entries[n].group = group;
            menu->entries[n].name = Script_ReadValue(pc);
            menu->entries[n].longitude = Script_GetValue();
            menu->entries[n].latitude = Script_GetValue();
        }
    }
    menu->page = menu->entries[menu->selected].group;
    res = ScriptProc_Start(0xCB05, 0xE2AB);
    ResourceHeader_Relocate(&menu->widgets[0].resource, res);
    Widget_InitLinked(&menu->widgets[0].sprite, &menu->widgets[0].resource, 0x35, 0x10, 0, 0, 0, 0);
    ResourceHeader_Relocate(&menu->widgets[1].resource, res);
    Widget_InitLinked(&menu->widgets[1].sprite, &menu->widgets[1].resource,
                      (u16)(menu->page + 0x18), 0x10, 0, 0, 0, 0);
    res = ScriptProc_Start(0xCB05, 0x530D);
    ResourceHeader_Relocate(&menu->widgets[2].resource, res);
    Widget_InitLinked(&menu->widgets[2].sprite, &menu->widgets[2].resource, 0x8a, 0x10, 0, 0, 0, 0);
    ResourceHeader_Relocate(&menu->widgets[3].resource, res);
    Widget_InitLinked(&menu->widgets[3].sprite, &menu->widgets[3].resource, 0x48, 0x10, 0, 0, 0, 0);
    menu->widgets[3].sprite.flags |= 1;
    Time_ShowRegionMenu(menu);
    {
        u16 **saved = &menu->savedPalette;
        u16 **pal = &menu->widgets[3].sprite.palette;
        u16 *copy;
        *saved = *pal;
        copy = menu->palette;
        CpuSet(*saved, copy, 0x10);
        *pal = copy;
    }
    Time_ShowRegionMenu(menu);
    rect.x = 0;
    rect.y = (menu->selected - menu->skipped) * 16 + 0x38;
    rect.z = 0;
    Menu_SetTextRect(&menu->widgets[3].sprite, &rect);
    menu->widgets[3].sprite.flags &= ~1;
    Menu_StartFadeIn(menu->fadeSpeed);
    return 0;
}

void Time_SetRegionMenuParametersFromScript(void)
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

struct RegionMenu *Time_CreateRegionMenu(void)
{
    struct RegionMenu *p = Task_Create(0xb, sizeof(*p));
    if (p) {
        Task_SetCallbacks(p, (void (*)(void *))Time_HandleRegionKeypad,
                          (void (*)(void *))Time_DestroyRegionMenu);
        if (Time_LoadRegionList(p) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}

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
    region = ((struct SessionSave *)gSessionState)->region;
    if (region - 1 > 52) {
        ((struct SessionSave *)gSessionState)->region = 6;
        region = 6;
    }
    Time_CalculateSunriseSunset(entries[region].latitude, entries[region].longitude, 0);
    gSunClock.pad9 = 0;
}
