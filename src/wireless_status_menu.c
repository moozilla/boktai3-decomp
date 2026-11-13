#include "wireless_status_menu.h"
#include "input.h"

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
    int zero;
    menu->background = resource;
    zero = 0;
    Bg_SetupLayers(1, 0, resource, 0, zero, 1, &layer);
    BgGfx_LoadTilemapSimple(2, 0, 0, 0, zero);
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
    {
        int bits = 4;
        u32 *flags;
        flags = &gEnginePauseFlags;
        *flags |= bits;
    }
    if (menu->entered) {
        menu->entered = 0;
        Wireless_RequestDisconnectMode();
    }
    if (menu->timer == 4) {
        Sound_FadeOutChannel10(1);
        WirelessStatusMenu_InitDisplay(menu);
        WirelessStatusMenu_LoadStatusBackground(menu, 2);
        menu->widget.flags &= ~1;
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

void WirelessStatusMenu_UpdateDismissibleMessageState(struct WirelessStatusMenu *menu)
{
    {
        int bits = 4;
        u32 *flags;
        flags = &gEnginePauseFlags;
        *flags |= bits;
    }
    if (menu->entered) {
        menu->entered = 0;
        Sound_FadeOutChannel10(1);
        WirelessStatusMenu_InitDisplay(menu);
        Wireless_RequestDisconnectMode();
        WirelessStatusMenu_LoadStatusBackground(menu, 2);
        menu->widget.flags &= ~1;
        menu->window = TextWindow_Create(1, 14, 28, 6);
        if (menu->window >= 0) {
            TextWindow_SetScript(menu->window, menu->messageTable);
            TextWindow_Refresh(menu->window, 1);
            TextWindow_StartPlayback(menu->window);
        }
        Sound_PlaySong(402);
    }
    if (menu->timer > 59 && (gPadState[1] & 11)) {
        {
            int state = 3;
            int zero = 0;
            menu->state = state;
            menu->entered = 1;
            menu->timer = zero;
        }
    } else
        menu->timer++;
}

void WirelessStatusMenu_UpdateExitState(struct WirelessStatusMenu *menu)
{
    if (menu->entered != 0) {
        u32 z = 0;
        menu->entered = z;
        ScreenTransition_SendCommand(3, 4, 4, 4, 4, 0xFFFF, z);
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
        if (state == 1) {
            menu->state = state;
            menu->entered = state;
            menu->timer = previous;
        } else if (state == 2) {
            menu->state = state;
            menu->entered = 1;
            menu->timer = previous;
        }
    }
    Callback_InvokeActor(menu, gUnk_08605770[menu->state]);
    return 0;
}

int WirelessStatusMenu_Destroy(struct WirelessStatusMenu *menu)
{
    TextWindow_Destroy(menu->window);
    Bg_ClearTilemap(0);
    Bg_ClearTilemap(2);
    Anim_Remove(&menu->widget);
    gLinkMenu = NULL;
    return 0;
}

int WirelessStatusMenu_InitFromScript(struct WirelessStatusMenu *menu)
{
    struct SceneInputResource *resource;
    struct SceneInputResource *blob;
    int zero;
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
        void *drawable = &menu->widget;
        int flags;
        flags = 0x2011;
        zero = 0;
        Widget_InitLinked(drawable, blob, 3, flags, zero, zero, 60, zero);
    }
    menu->completionProcedure = Script_GetKeywordValue('r', 0);
    menu->window = -1;
    menu->state = zero;
    menu->entered = 1;
    menu->timer = zero;
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
