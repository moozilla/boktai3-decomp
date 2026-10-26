#include "global.h"
#include "render.h"
struct RegionEntry {
    s16 group;
    u16 name;
    s32 longitude;
    s32 latitude;
};
struct MenuAnim {
    u8 widget[8];
    u32 flags;
    u8 pad0c[0x48 - 0xc];
    void *palette;
    u8 pad4c[0x60 - 0x4c];
    u8 resource[0x28];
};
struct RegionMenu {
    u8 pad0[0x1c];
    s32 page;
    s32 count;
    u8 pad24[2];
    s8 selected;
    u8 pulse;
    s32 pageFirst[14];
    struct MenuAnim anims[4];
    struct RegionEntry entries[64];
    void *savedPalette;
    u16 palette[16];
    s32 state;
    s32 confirmed;
    s32 timer;
    s32 fadeSpeed;
    s32 skipped;
};
struct Rect16 {
    u32 x : 16;
    u32 y : 16;
    u32 z : 16;
    u32 w : 16;
};
struct Params310 {
    u32 unk0, table, confirmProc, cancelProc, flag;
};
struct TimeState {
    u8 pad0[0x18];
    u32 region;
};
extern struct Params310 gUnk_02000310;
extern struct TimeState *gUnk_030053F8;
void Bg_ClearTilemap(u32);
void BgGfx_LoadTilemapSimple(u32, u32, u32, u32, u32);
void *ScriptProc_Start(u32, u32);
void Bg_SetupLayers(u32, u32, void *, u32, u32, u32, u32 *);
void ResourceHeader_Relocate(void *, void *);
void Widget_InitLinked(void *, void *, u32, u32, u32, u32, u32, u32);
void Menu_StartFadeIn(u32);
void Menu_SetTextRect(void *, struct Rect16 *);
u8 *Script_SetPc(u8 *pc);
u8 *Script_GetPc(void);
s32 Script_GetValue(void);
s32 Script_ReadValue(void);
void Time_ShowRegionMenu(struct RegionMenu *);
u32 Time_LoadRegionList(struct RegionMenu *menu)
{
    s32 n, k, count, group;
    u32 zero;
    void *res;
    void *tiles;
    u32 value;
    struct Rect16 rect;
    if (gUnk_02000310.flag == 1)
        menu->selected = 6;
    else
        menu->selected = gUnk_030053F8->region;
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
    Script_SetPc((u8 *)gUnk_02000310.table);
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
        copy = menu->palette;
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
