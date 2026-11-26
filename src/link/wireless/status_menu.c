#include "link/wireless/status_menu.h"
#include "engine/input.h"
#include "engine/engine_pause_flags.h"

struct WirelessStatusMenu *WirelessStatusMenu_Get(void)
{
    return (void *)gLinkMenu;
}

void WirelessStatusMenu_InitDisplay(struct WirelessStatusMenu *menu)
{
    TextWindowManager_DestroyAllWindows();
    PaletteLevels_Reset();
    {
        u16 *shadow = &gDispCntShadow;
        u32 mask;
        mask = ~0x600;
        mask &= *shadow;
        *shadow = mask;
    }
    SpriteOam_ResetQueue(1);
    Video_SetRenderCallbacks(0, Video_OnFrameNoOp, Video_OnRenderNoOp, sub_0821969C);
    Hud_SetMode(7);
    Bg_ClearTilemap(0);
}

void WirelessStatusMenu_LoadPrimaryBackground(struct WirelessStatusMenu *menu, u32 index)
{
    struct PaletteResource *palette;
    menu->background = ScriptProc_Start(0xc091, 0xa597);
    Bg_SetupLayers(0, 0, menu->background, 0, 0, 1, &index);
    BgGfx_LoadTilemapSimple(0, 0, 0, 0, 0);
    palette = ScriptProc_Start(0x92b3, 0x81b);
    CpuSet(palette->banks, gBgPaletteBuffer, 0x100);
}

void WirelessStatusMenu_LoadStatusBackground(struct WirelessStatusMenu *menu, int layer)
{
    void *resource = ScriptProc_Start(0xC091, 0xCF15);
    menu->background = resource;
    Bg_SetupLayers(1, 0, resource, 0, 0, 1, &layer);
    BgGfx_LoadTilemapSimple(2, 0, 0, 0, 0);
    {
        struct PaletteResource *palette = ScriptProc_Start(0x92B3, 0x26BB);
        CpuSet(palette->banks, gBgPaletteBuffer, 256);
    }
}

void WirelessStatusMenu_UpdateIdleState(struct WirelessStatusMenu *menu)
{
    if (menu->entered)
        menu->entered = 0;
    menu->timer++;
}


void WirelessStatusMenu_UpdateDisconnectState(struct WirelessStatusMenu *menu)
{
    Engine_SetPauseFlags(4);
    if (menu->entered) {
        menu->entered = 0;
        Wireless_RequestDisconnectMode();
    }
    if (menu->timer == 4) {
        Sound_FadeOutChannel10(1);
        WirelessStatusMenu_InitDisplay(menu);
        WirelessStatusMenu_LoadStatusBackground(menu, 2);
        menu->animSprite.flags &= ~1;
        menu->window = TextWindow_Create(1, 14, 28, 6);
        if (menu->window >= 0) {
            TextWindow_SetScript(menu->window, menu->messageTable);
            TextWindow_Refresh(menu->window, 0);
            TextWindow_StartPlayback(menu->window);
        }
        Sound_PlaySong(402);
    }
    menu->timer++;
}

static inline void WirelessStatusMenu_SetState(struct WirelessStatusMenu *menu, u8 state)
{
    menu->state = state;
    menu->entered = 1;
    menu->timer = 0;
}

void WirelessStatusMenu_UpdateDismissibleMessageState(struct WirelessStatusMenu *menu)
{
    Engine_SetPauseFlags(4);
    if (menu->entered) {
        menu->entered = 0;
        Sound_FadeOutChannel10(1);
        WirelessStatusMenu_InitDisplay(menu);
        Wireless_RequestDisconnectMode();
        WirelessStatusMenu_LoadStatusBackground(menu, 2);
        menu->animSprite.flags &= ~1;
        menu->window = TextWindow_Create(1, 14, 28, 6);
        if (menu->window >= 0) {
            TextWindow_SetScript(menu->window, menu->messageTable);
            TextWindow_Refresh(menu->window, 1);
            TextWindow_StartPlayback(menu->window);
        }
        Sound_PlaySong(402);
    }
    if (menu->timer > 59 && (gPadState[1] & 11)) {
        WirelessStatusMenu_SetState(menu, 3);
    } else
        menu->timer++;
}

void WirelessStatusMenu_UpdateExitState(struct WirelessStatusMenu *menu)
{
    if (menu->entered != 0) {
        menu->entered = 0;
        ScreenTransition_SendCommand(3, 4, 4, 4, 4, 0xFFFF, 0);
        Sound_PlaySong(0xde);
        Wireless_RequestResetConnection();
    }
    if (menu->timer == 0x14) {
        Video_SetRenderCallbacks(0, SpriteOam_QueueProjectedSprites, Sprite_RenderScaledList,
                                 sub_08219624);
        if (menu->completionProcedure != 0)
            Script_Call(menu->completionProcedure, 0);
    }
    menu->timer++;
}

int WirelessStatusMenu_Update(struct WirelessStatusMenu *menu)
{
    int previous = menu->state;
    if (!previous) {
        int state = Wireless_GetStatus();
        if (state == 1)
            WirelessStatusMenu_SetState(menu, 1);
        else if (state == 2)
            WirelessStatusMenu_SetState(menu, 2);
    }
    Callback_InvokeActor(menu, gUnk_08605770[menu->state]);
    return 0;
}

int WirelessStatusMenu_Destroy(struct WirelessStatusMenu *menu)
{
    TextWindow_Destroy(menu->window);
    Bg_ClearTilemap(0);
    Bg_ClearTilemap(2);
    Anim_Remove(&menu->animSprite);
    gLinkMenu = NULL;
    return 0;
}

int WirelessStatusMenu_InitFromScript(struct WirelessStatusMenu *menu)
{
    struct AnimSpriteResource *resource;
    struct AnimSpriteResource *blob;
    gLinkMenu = (void *)menu;
    PaletteLevels_Reset();
    if (!Script_SeekToKeyword('s'))
        return -1;
    menu->messageTable = Script_GetTextSourcePc();
    if (!menu->messageTable)
        return -1;
    resource = ScriptProc_Start(0xCB05, 0x7C8B);
    if (!resource)
        return -1;
    menu->resource = *resource;
    blob = &menu->resource;
    ResourceHeader_Relocate(blob, resource);
    {
        void *drawable = &menu->animSprite;
        AnimSprite_InitLinked(drawable, blob, 3, 0x2011, 0, 0, 60, 0);
    }
    menu->completionProcedure = Script_GetKeywordValue('r', 0);
    menu->window = -1;
    WirelessStatusMenu_SetState(menu, 0);
    return 0;
}

struct WirelessStatusMenu *WirelessStatusMenu_Create(void)
{
    struct WirelessStatusMenu *menu = WirelessStatusMenu_Get();
    if (menu)
        return menu;
    menu = Task_Create(12, sizeof(*menu));
    if (menu) {
        Task_SetCallbacks(menu, (void (*)(void *))WirelessStatusMenu_Update,
                          (void (*)(void *))WirelessStatusMenu_Destroy);
        if (WirelessStatusMenu_InitFromScript(menu) < 0) {
            Task_Kill(menu);
            return 0;
        }
    }
    return menu;
}
