#include "link/message_panel/message_panel.h"
#include "engine/input.h"
#include "engine/runtime.h"
#include "engine/video/palette_resource.h"
#include "gba/syscall.h"
#include "script/script_utility.h"

void Bg_SetupLayers(u32, u32, void *, u32, u32, u32, u32 *);

u32 TextWindow_Create(u32, u32, u32, u32);

void TextWindow_StartPlayback(s32);

void TextWindow_Clear(s32);

void ScreenTransition_SendCommand(s32, s32, s32, s32, s32, s32, s32);

u8 Save_CopySelectedStateToAlternateSlot(void);

u8 Save_ToggleSelectedSlot(void);

void System_ShutdownAndSoftReset(void);

void sub_08108FB4(s32, s32, s32, s32, u32);

void LinkConnection_SetCallbacks(void *, void (*)(struct MessagePanel *, u32, u32, u32),
                                 void (*)(struct MessagePanel *, u32, s32, u32));

void Solar_PauseMeasurement(void);

void Solar_ResumeMeasurement(void);

u32 Script_GetTextSourcePc(void);

static inline u8 MessagePanel_IsBlocking(struct MessagePanel *panel)
{
    if (panel->flags & 1)
        return TRUE;
    return FALSE;
}

void MessagePanel_LoadBackground(struct MessagePanel *panel)
{
    u32 maps[1];
    void *resource;
    void *palette;
    resource = ScriptProc_Start(0xC091, 0xA597);
    panel->backgroundResource = resource;
    maps[0] = 3;
    Bg_SetupLayers(2, 0, resource, 0, 0, 1, maps);
    palette = ((struct PaletteResource *)ScriptProc_Start(0x92B3, 0x81B))->banks;
    panel->backgroundPalette = palette;
    CpuCopy16(palette, gBgPaletteBuffer, 0x200);
}

void MessagePanel_CreateTextWindow(struct MessagePanel *panel)
{
    u32 window;

    window = TextWindow_Create(1, 6, 0x1C, 6);
    panel->window = window;
    TextWindow_SetScript(window, panel->script);
    TextWindow_Refresh(panel->window, 0);
    TextWindow_StartPlayback(panel->window);
}

void MessagePanel_HideTextWindow(struct MessagePanel *panel)
{
    TextWindow_Clear(panel->window);
}

void MessagePanel_RunExitScript(struct MessagePanel *panel)
{
    u32 v;

    Bg_ClearTilemap(0);
    Bg_ClearTilemap(1);
    Bg_ClearTilemap(2);
    Bg_ClearTilemap(3);
    v = panel->exitScript;
    if (v != 0) {
        Script_Call(v, 0);
    }
}

void MessagePanel_ShowOutcome(struct MessagePanel *panel, u32 unused, s32 success, u32 unusedResult)
{
    if (success) {
        TextWindow_Clear(panel->window);
        TextWindow_SetScript(panel->window, panel->script);
        TextWindow_Refresh(panel->window, 2);
        TextWindow_StartPlayback(panel->window);
        MessagePanel_SetState(panel, MessagePanel_DelayDismiss, 2);
        Sound_PlaySong(0xde);
    } else {
        TextWindow_Clear(panel->window);
        TextWindow_SetScript(panel->window, panel->closeScript);
        TextWindow_Refresh(panel->window, 3);
        TextWindow_StartPlayback(panel->window);
        MessagePanel_SetState(panel, MessagePanel_DelayDismiss, 2);
        Sound_PlaySong(0x192);
    }
}

void MessagePanel_RunOutcomeScript(struct MessagePanel *panel, u32 context, u32 unusedOperation,
                                   u32 result)
{
    s32 value;
    struct ScriptUtilityArguments args;
    if (result == 0) {
        MessagePanel_ShowOutcome(panel, context, 0, 0);
    } else {
        panel->result = result;
        if (result <= 31)
            *(u32 *)gGameState->unk820 |= 1 << result;
        if (panel->outcomeScript) {
            value = panel->result;
            args.count = 1;
            args.values = &value;
            Script_Call(panel->outcomeScript, &args);
            MessagePanel_SetState(panel, MessagePanel_ShowNotice, 5);
        }
    }
}

s32 MessagePanel_GetErrorMessageIndex(struct MessagePanel *panel)
{
    u32 v = panel->result;
    switch (v) {
        case 3:
            return 0;
        case 0x10:
            return 1;
        default:
            return 2;
    }
}

void MessagePanel_ShowNotice(struct MessagePanel *panel)
{
    if (panel->entering == 1) {
        TextWindow_Clear(panel->window);
        TextWindow_SetScript(panel->window, panel->closeScript);
        TextWindow_Refresh(panel->window, MessagePanel_GetErrorMessageIndex(panel));
        TextWindow_StartPlayback(panel->window);
        Sound_PlaySong(0x123);
        panel->timer = 0;
        panel->entering = 0;
    }
    if (panel->timer > 120) {
        MessagePanel_SetState(panel, MessagePanel_ShowFollowup, 6);
    }
    panel->timer++;
}

void MessagePanel_ShowFollowup(struct MessagePanel *panel)
{
    if (panel->entering == 1) {
        TextWindow_Clear(panel->window);
        TextWindow_SetScript(panel->window, panel->script);
        TextWindow_Refresh(panel->window, 4);
        TextWindow_StartPlayback(panel->window);
        panel->timer = 0;
        panel->entering = 0;
    }
    if (panel->timer == 120) {
        if (!Save_CopySelectedStateToAlternateSlot()) {
            MessagePanel_SetState(panel, MessagePanel_WaitForInput, 8);
        } else {
            MessagePanel_SetState(panel, MessagePanel_SelectNextState, 9);
        }
    } else {
        panel->timer++;
    }
}

void MessagePanel_SelectNextState(struct MessagePanel *panel)
{
    if (Save_ToggleSelectedSlot()) {
        MessagePanel_SetState(panel, MessagePanel_WaitForDismiss, 7);
    } else {
        MessagePanel_SetState(panel, MessagePanel_WaitForInput, 8);
    }
}

void MessagePanel_WaitForDismiss(struct MessagePanel *panel)
{
    if (panel->entering) {
        TextWindow_SetScript(panel->window, panel->script);
        TextWindow_Refresh(panel->window, 5);
        Sound_PlaySong(0x2AE);
        panel->timer = 0;
        panel->entering = 0;
    }
    if (panel->timer == 120) {
        MessagePanel_SetState(panel, MessagePanel_BeginDismiss, 3);
    } else {
        panel->timer++;
    }
}

void MessagePanel_WaitForInput(struct MessagePanel *panel)
{
    if (panel->entering) {
        TextWindow_SetScript(panel->window, panel->script);
        TextWindow_Refresh(panel->window, 3);
        Sound_PlaySong(0x192);
        panel->timer = 0;
        panel->entering = 0;
    }
    if (panel->timer < 60)
        panel->timer++;
    else if (gPadState[1] & 0x30F)
        System_ShutdownAndSoftReset();
}

void MessagePanel_OnExitState(struct MessagePanel *panel)
{
    if (panel->entering == 1) {
        MessagePanel_RunExitScript(panel);
        panel->entering = 0;
    }
}

void MessagePanel_BeginDismiss(struct MessagePanel *panel)
{
    if (panel->entering == 1) {
        ScreenTransition_SendCommand(3, 4, 4, 4, 4, 0xFFFF, 0);
        panel->timer = 0;
        panel->entering = 0;
    }
    if (panel->timer == 60) {
        MessagePanel_SetState(panel, MessagePanel_OnExitState, 4);
    } else
        panel->timer++;
}

void MessagePanel_DelayDismiss(struct MessagePanel *panel)
{
    if (panel->entering == 1) {
        panel->timer = 0;
        panel->entering = 0;
    }
    if (panel->timer == 120) {
        MessagePanel_SetState(panel, MessagePanel_BeginDismiss, 3);
    } else {
        panel->timer++;
    }
}

void MessagePanel_ResetDisplay(struct MessagePanel *panel)
{
    if (panel->entering == 1) {
        BgGfx_LoadTilemapSimple(3, 0, 0, 0, 0);
        panel->entering = 0;
        sub_08108FB4(-1, 1, 0, 0x36, panel->connectionKey);
        LinkConnection_SetCallbacks(panel, MessagePanel_RunOutcomeScript, MessagePanel_ShowOutcome);
        panel->timer = 0;
    }
    if (panel->timer < 30)
        panel->timer++;
}

s32 MessagePanel_Update(struct MessagePanel *panel)
{
    Solar_PauseMeasurement();
    gClockSynced = 1;
    if (MessagePanel_IsBlocking(panel))
        gSceneDialogActive = 1;
    panel->state(panel);
    return 1;
}

s32 MessagePanel_OnDestroy(struct MessagePanel *panel)
{
    Solar_ResumeMeasurement();
    gClockSynced = 0;
    MessagePanel_HideTextWindow(panel);
    return 1;
}

s32 MessagePanel_InitFromScript(struct MessagePanel *panel)
{
    if (Script_SeekToKeyword('r')) {
        panel->script = (const u8 *)Script_GetTextSourcePc();
        if (!panel->script)
            return -1;
    }
    if (Script_SeekToKeyword('d')) {
        panel->closeScript = (const u8 *)Script_GetTextSourcePc();
        if (!panel->closeScript)
            return -1;
    }
    panel->connectionKey = Script_GetKeywordValue('k', 0);
    panel->exitScript = Script_GetKeywordValue('e', 0);
    panel->outcomeScript = Script_GetKeywordValue('i', 0);
    MessagePanel_LoadBackground(panel);
    MessagePanel_CreateTextWindow(panel);
    MessagePanel_SetState(panel, MessagePanel_ResetDisplay, 0);
    if (Script_GetKeywordValue('f', 0))
        ScreenTransition_SendCommand(2, 4, 4, 4, 4, 0xFFFF, 0);
    panel->flags |= 1;
    return 1;
}

struct MessagePanel *MessagePanel_CreateFromScript(void)
{
    struct MessagePanel *panel;
    panel = Task_Create(11, sizeof(*panel));
    if (panel != 0) {
        Task_SetCallbacks(panel, (void (*)(void *))MessagePanel_Update,
                          (void (*)(void *))MessagePanel_OnDestroy);
        if (MessagePanel_InitFromScript(panel) < 0) {
            Task_Kill(panel);
            return 0;
        }
    }
    return panel;
}
