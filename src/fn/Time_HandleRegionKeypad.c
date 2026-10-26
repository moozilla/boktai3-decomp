#include "global.h"
#include "render.h"
struct RegionEntry {
    s16 group;
    u16 name;
    s32 longitude;
    s32 latitude;
};
struct MenuAnim {
    u8 pad0[8];
    u32 flags;
    u8 padc[0x88 - 0xc];
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
    u8 pad280[0];
    struct RegionEntry entries[(0x59c - 0x280) / 12];
    u8 pad598[4];
    u8 pulseState[8];
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
struct ScriptCallArgs {
    u32 count : 16;
    u32 unused : 16;
    s32 *values;
};
struct Params310 {
    u32 unk0, unk4, confirmProc, cancelProc, flag;
};
extern struct Params310 gUnk_02000310;
extern u16 gUnk_03005260[];
void Sound_PlaySong(u32);
void Time_OnRegionListSelect(struct RegionMenu *);
void Time_ShowRegionMenu(struct RegionMenu *);
void Menu_StartFadeOut(s32);
s32 Anim_LoadKeyframe(void *, void *, u32, u32);
void Menu_SetTextRect(void *, struct Rect16 *);
void Palette_UpdateHighlightPulse(void *, u8 *);
void Script_Call(u32, struct ScriptCallArgs *);
void Task_Kill(void *);
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
            if (gUnk_03005260[1] & 1) {
                Sound_PlaySong(0xdd);
                if (gUnk_02000310.flag == 1)
                    menu->fadeSpeed = 3;
                else
                    menu->fadeSpeed = 0;
                Time_OnRegionListSelect(menu);
                Menu_StartFadeOut(menu->fadeSpeed);
                menu->state = 1;
                menu->confirmed = 1;
            } else if (gUnk_03005260[1] & 2) {
                Sound_PlaySong(0xde);
                Menu_StartFadeOut(menu->fadeSpeed);
                menu->state = 1;
                menu->confirmed = 0;
            } else {
                u8 old;
                if (gUnk_03005260[1] & 0x100) {
                    if (++menu->page > 13)
                        menu->page = 1;
                    Sound_PlaySong(0xdc);
                    menu->selected = menu->pageFirst[menu->page];
                    Anim_LoadKeyframe(&menu->anims[1], (u8 *)menu + 0x148, (u16)(menu->page + 0x18),
                                      0);
                    Time_ShowRegionMenu(menu);
                }
                if (gUnk_03005260[1] & 0x200) {
                    if (--menu->page <= 0)
                        menu->page = 13;
                    Sound_PlaySong(0xdc);
                    menu->selected = menu->pageFirst[menu->page];
                    Anim_LoadKeyframe(&menu->anims[1], (u8 *)menu + 0x148, (u16)(menu->page + 0x18),
                                      0);
                    Time_ShowRegionMenu(menu);
                }
                if (gUnk_03005260[1] & 0x40) {
                    old = menu->selected;
                    menu->selected = old - 1;
                    if (menu->entries[menu->selected].group != menu->page)
                        menu->selected = old;
                    else
                        Sound_PlaySong(0xdc);
                }
                if (gUnk_03005260[1] & 0x80) {
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
                Palette_UpdateHighlightPulse(menu->pulseState, &menu->pulse);
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
                    Script_Call(gUnk_02000310.cancelProc, &args);
                else
                    Script_Call(gUnk_02000310.confirmProc, &args);
                Task_Kill(menu);
            }
            menu->timer++;
            break;
        case 2:
            break;
    }
}
