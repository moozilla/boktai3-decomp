#include "link/panel/link_panel.h"
#include "scene/areas/area_effect_link.h"
#include "ui/dialogue/scrolling_dialogue.h"
#include "engine/input.h"
#include "engine/ram.h"

void LinkPanel_SelectActiveAnimation(struct LinkPanel *, s32);

void LinkPanelSlots_ShowSlotReadyFrame(struct LinkPanel *, s32);

void LinkPanelSlots_ShowSlotDefaultFrame(struct LinkPanel *, s32);

s32 LinkSession_GetSessionState(void);

static inline u8 valid(s32 index)
{
    if (index < 0)
        return 0;
    if (index > 1)
        return 0;
    return 1;
}

static inline u8 connected(void)
{
    if (gLinkSession && gLinkSession->exchangeBusy)
        return 1;
    return 0;
}

static inline s32 peerStatus(s32 index)
{
    if (!gLinkSession)
        return 0;
    return gLinkSession->unknown64[index];
}

static inline u8 delayMessage(struct LinkPanel *panel)
{
    if (panel->messageDelay <= 30) {
        panel->messageDelay++;
        return 1;
    }
    return 0;
}

void LinkPanel_UpdateAnimations(struct LinkPanel *panel)
{
    s32 i;
    struct AreaEffectLinkState *peer;
    for (i = 0; i <= 1; i++) {
        peer = gLinkSession;
        if (peer && peer->unavailable == 1 && valid(i) &&
            &peer->sessionData[0x40 + i * sizeof(struct AreaEffectLinkResourceSlot)]) {
            if (panel->entries[i].status.animationTimer > 30) {
                if (panel->entries[i].status.currentStatus > 1)
                    LinkPanel_SelectActiveAnimation(panel, i);
                else if (panel->entries[i].status.currentStatus == 1)
                    LinkPanelSlots_ShowSlotReadyFrame(panel, i);
                else
                    goto inactive;
            } else
                goto increment;
        } else {
        inactive:
            LinkPanelSlots_ShowSlotDefaultFrame(panel, i);
        }
        goto step;
    increment:
        panel->entries[i].status.animationTimer++;
    step:
        if (panel->selected[i] != panel->previous[i]) {
            AnimSprite_SelectGlyph(&panel->animations[i], &panel->entryResource,
                                   panel->selected[i]);
            panel->previous[i] = panel->selected[i];
        }
        Anim_Step(&panel->animations[i], &panel->entryResource);
    }
}

void LinkPanel_UpdateLastEntry(struct LinkPanel *panel)
{
    if (panel->entries[0].status.currentStatus == 3)
        panel->selected[6] = 0x8E;
    else
        panel->selected[6] = 0x7F;
    if (panel->selected[6] != panel->previous[6]) {
        AnimSprite_SelectGlyph(&panel->animations[6], &panel->lastEntryResource,
                               panel->selected[6]);
        panel->previous[6] = panel->selected[6];
    }
}

void LinkPanel_InitMessageDisplay(struct LinkPanel *unused)
{
    ScrollingDialogue_ClearScript();
    ScrollingDialogue_RequestShow();
}

s32 LinkPanel_GetConnectionStatus(struct LinkPanel *panel)
{
    s32 state, result, status;
    if (panel->role < 0) {
    two:
        return 2;
    }
    state = LinkSession_GetSessionState();
    if ((u32)(state - 5) <= 1)
        return 4;
    if (connected() && state == 8)
        return 6;
    status = peerStatus(panel->role);
    if (status == 2)
        goto two;
    if (status <= 2)
        goto lower;
    if (status == 3)
        goto three;
    goto bad;
lower:
    if (status < 0)
        goto bad;
    result = 1;
    goto end;
three:
    result = 3;
    goto end;
bad:
    result = -1;
end:
    return result;
}

void LinkPanel_UpdateStatusMessage(struct LinkPanel *panel)
{
    s32 state, status, connection;
    connection = LinkPanel_GetConnectionStatus(panel);
    if (connection < 0 || panel->connection != connection) {
        panel->messageDelay = 0;
        panel->connection = connection;
    }
    if (panel->role < 0) {
        if ((s8)panel->message == 2)
            return;
        if (delayMessage(panel))
            return;
        ScrollingDialogue_ClearScript();
        ScrollingDialogue_SetScript(
            Text_LookupString(Script_ParseStringRef(panel->messageScript) + 2));
        panel->message = 2;
        return;
    }
    state = LinkSession_GetSessionState();
    if ((u32)(state - 5) <= 1) {
        if ((s8)panel->message == 4)
            return;
        if (delayMessage(panel))
            return;
        ScrollingDialogue_ClearScript();
        ScrollingDialogue_SetScript(
            Text_LookupString(Script_ParseStringRef(panel->messageScript) + 4));
        panel->message = 4;
        return;
    }
    if (connected() && state == 8) {
        if ((s8)panel->message == 6)
            return;
        if (delayMessage(panel))
            return;
        ScrollingDialogue_ClearScript();
        ScrollingDialogue_SetScript(
            Text_LookupString(Script_ParseStringRef(panel->messageScript) + 6));
        panel->message = 6;
        return;
    }
    status = peerStatus(panel->role);
    switch (status) {
        case 0:
        case 1:
            if ((s8)panel->message == 1)
                return;
            if (delayMessage(panel))
                return;
            ScrollingDialogue_ClearScript();
            ScrollingDialogue_SetScript(
                Text_LookupString(Script_ParseStringRef(panel->messageScript) + 1));
            panel->message = 1;
            break;
        case 2:
            if ((s8)panel->message == 2)
                return;
            if (delayMessage(panel))
                return;
            ScrollingDialogue_ClearScript();
            ScrollingDialogue_SetScript(
                Text_LookupString(Script_ParseStringRef(panel->messageScript) + 2));
            panel->message = 2;
            break;
        case 3:
            if (panel->number != panel->oldNumber) {
                ScrollingDialogue_ClearScript();
                ScrollingDialogue_SetNumberArgument(0, panel->number);
                ScrollingDialogue_SetScript(
                    Text_LookupString(Script_ParseStringRef(panel->messageScript) + 3));
                panel->message = status;
            } else {
                if ((s8)panel->message == 3)
                    return;
                if (delayMessage(panel))
                    return;
                ScrollingDialogue_ClearScript();
                ScrollingDialogue_SetNumberArgument(0, panel->number);
                ScrollingDialogue_SetScript(
                    Text_LookupString(Script_ParseStringRef(panel->messageScript) + 3));
                panel->message = 3;
            }
            break;
    }
}

void LinkPanelSlots_HideAnimations(struct LinkPanel *);

void LinkPanelSlots_RemoveAnimations(void *);

void LinkSession_NotifyMenuClosed(void);

void PaletteFade_CycleMenuColors(void *);

s32 Serial_GetRole(void);

void LinkPanelSlots_InitBackgroundResources(struct LinkPanel *);

void SlotMenu_InitializeEntryValues(struct LinkPanel *);

void SceneMenu_InitializeDisplay(struct LinkPanel *);

void sub_0810C0BC(struct LinkPanel *);

void ModifierMenu_InitSelectionSnapshot(void *, void *);

void SlotMenu_CreateTextWindows(struct LinkPanel *);

void *Script_GetTextSourcePc(void);

static inline u8 unavailable(void)
{
    if (gLinkSession && gLinkSession->menuInputLocked == 1)
        return 1;
    return 0;
}

static inline s32 GetPeer(void)
{
    struct AreaEffectLinkState *session = gLinkSession;
    s32 result = -1;
    if (session)
        result = session->participantCount;
    return result;
}

static inline void SetUpdate(struct LinkPanel *panel, void (*update)(struct LinkPanel *), u32 state)
{
    panel->update = update;
    panel->redraw = 1;
    panel->state = state;
}

void LinkPanel_Close(struct LinkPanel *panel)
{
    ScrollingDialogue_ClearScript();
    ScrollingDialogue_HideImmediately();
    Bg_ClearTilemap(0);
    Bg_ClearTilemap(1);
    Bg_ClearTilemap(2);
    Bg_ClearTilemap(3);
    LinkPanelSlots_HideAnimations(panel);
    LinkSession_NotifyMenuClosed();
    if (panel->exitScript)
        Script_Call(panel->exitScript, 0);
    Task_Kill(panel);
}

void LinkPanel_RunCompletionScript(struct LinkPanel *panel)
{
    u32 script = panel->completionScript;
    if (script) {
        Script_Call(script, 0);
        Task_Kill(panel);
    }
}

void LinkPanel_UpdateExitTransition(struct LinkPanel *panel)
{
    if (panel->redraw) {
        ScreenTransition_SendCommand(3, 4, 4, 4, 4, 0xFFFF, 0);
        panel->timer = 0;
        panel->redraw = 0;
    }
    if (panel->timer == 10)
        LinkPanel_Close(panel);
    panel->timer++;
}

void LinkPanel_UpdateConnection(struct LinkPanel *panel)
{
    s32 i;
    if (panel->redraw) {
        panel->message = 0xff;
        Bg_SetLayerMap(3, panel->backgroundResource, 2);
        BgGfx_LoadTilemapSimple(3, 0, 0, 0, 0);
        panel->timer = 0;
        panel->redraw = 0;
        for (i = 0; i < 2; i++) {
            panel->entries[i].status.previousStatus = panel->entries[i].status.currentStatus;
            panel->entries[i].status.currentStatus = 0;
            panel->entries[i].status.animationTimer = 0;
            LinkPanelSlots_ShowSlotDefaultFrame(panel, i);
        }

        panel->messageDelay = 0;
        ScrollingDialogue_ClearScript();
        ScrollingDialogue_SetScript(
            Text_LookupString(Script_ParseStringRef(panel->messageScript) + 2));
        panel->message = 2;
        panel->connection = 2;
    }
    panel->role = Serial_GetRole();
    panel->number = GetPeer();
    for (i = 0; i <= 1; i++) {
        panel->entries[i].status.previousStatus = panel->entries[i].status.currentStatus;
        if (panel->role < 0)
            panel->entries[i].status.currentStatus = 0;
        else
            panel->entries[i].status.currentStatus = peerStatus(i);
        if (panel->entries[i].status.previousStatus != panel->entries[i].status.currentStatus)
            panel->entries[i].status.animationTimer = 0;
    }
    LinkPanel_UpdateLastEntry(panel);
    LinkPanel_UpdateAnimations(panel);
    PaletteFade_CycleMenuColors(&panel->paletteCycle);
    LinkPanel_UpdateStatusMessage(panel);
    panel->oldNumber = panel->number;
    if (panel->timer <= 0x3b) {
        panel->timer++;
    } else if (!unavailable()) {
        if (gPadState[1] & 2) {
            SetUpdate(panel, LinkPanel_UpdateExitTransition, 1);
            Sound_PlaySong(0xde);
        } else {
            panel->animations[6].flags &= ~1;
        }
    } else {
        panel->animations[6].flags |= 1;
    }
}

s32 LinkPanel_Update(struct LinkPanel *panel)
{
    if (LinkSession_GetSessionState() == 0) {
        ScrollingDialogue_HideImmediately();
        Task_Kill(panel);
        return -1;
    }
    if (panel->update != 0)
        panel->update(panel);
    return 1;
}

s32 LinkPanel_Destroy(void *panel)
{
    LinkPanelSlots_RemoveAnimations(panel);
    gLinkPanel = 0;
    return 1;
}

s32 LinkPanel_InitFromScript(struct LinkPanel *panel)
{
    if (Script_SeekToKeyword('s')) {
        panel->messageScript = Script_GetTextSourcePc();
        if (!panel->messageScript)
            return -1;
    }
    if (Script_SeekToKeyword('c')) {
        panel->selectionScript = Script_GetTextSourcePc();
        if (!panel->selectionScript)
            return -1;
    }
    panel->exitScript = Script_GetKeywordValue('e', 0);
    Bg_ClearTilemap(3);
    Bg_ClearTilemap(2);
    Bg_ClearTilemap(1);
    Bg_ClearTilemap(0);
    LinkPanelSlots_InitBackgroundResources(panel);
    SlotMenu_InitializeEntryValues(panel);
    SceneMenu_InitializeDisplay(panel);
    sub_0810C0BC(panel);
    ModifierMenu_InitSelectionSnapshot(&panel->paletteCycle, panel->paletteResource);
    SlotMenu_CreateTextWindows(panel);
    LinkPanel_InitMessageDisplay(panel);
    SetUpdate(panel, LinkPanel_UpdateConnection, 0);
    if (Script_GetKeywordValue('f', 0))
        ScreenTransition_SendCommand(2, 5, 4, 4, 4, 0xFFFF, 0);
    gLinkPanel = (u32)panel;
    return 1;
}

struct LinkPanel *LinkPanel_Create(void)
{
    struct LinkPanel *panel;
    if (gLinkPanel != 0)
        return (struct LinkPanel *)gLinkPanel;
    panel = Task_Create(0xb, 0xf78);
    if (panel != 0) {
        Task_SetCallbacks(panel, (void (*)(void *))LinkPanel_Update,
                          (void (*)(void *))LinkPanel_Destroy);
        if (LinkPanel_InitFromScript(panel) < 0) {
            Task_Kill(panel);
            return 0;
        }
    }
    return panel;
}
