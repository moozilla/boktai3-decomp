#include "global.h"
#include "scene/areas/area_effect_link.h"
#include "ui/save/save_menu.h"
void LinkScorePanel_HideSprites(struct SaveMenu *);
void Bg_ClearTilemap(s32);
void Bg_SetLayerMap(s32, void *, s32);
void TextWindow_Clear(u32);
void TextWindow_SetBounds(u32, s32, s32, s32, s32);
void TextWindow_StartPlayback(u32);
void LinkSession_RequestHandshakeStart(void);
static inline void SetUpdate(struct SaveMenu *menu, void (*update)(struct SaveMenu *), u8 state)
{
    menu->update = update;
    menu->entering = 1;
    menu->state = state;
}

void LinkScorePanel_OnWaitSessionReady(struct SaveMenu *menu)
{
    struct AreaEffectLinkState *link;
    s32 state;
    if (menu->entering != 0) {
        LinkScorePanel_HideSprites(menu);
        Bg_ClearTilemap(0);
        Bg_ClearTilemap(3);
        Bg_SetLayerMap(0, menu->alternateBackgroundResource, 3);
        BgGfx_LoadTilemapSimple(0, 0, 0, 0, 0);
        TextWindow_Clear(menu->windows[0]);
        TextWindow_SetBounds(menu->windows[0], 1, 6, 0x1c, 6);
        TextWindow_SetScript(menu->windows[0], menu->titleScript);
        TextWindow_Refresh(menu->windows[0], 8);
        TextWindow_StartPlayback(menu->windows[0]);
        LinkSession_RequestHandshakeStart();
        menu->entering = 0;
    }
    link = gLinkSession;
    state = -1;
    if (link != NULL)
        state = link->unknown1C;
    if (state == 0x19)
        SetUpdate(menu, SaveMenu_UpdateSaveDelay, 4);
}
