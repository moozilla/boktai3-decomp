#include "link_panel.h"
#include "area_effect_link.h"
#include "scrolling_dialogue.h"

void LinkPanel_SelectActiveAnimation(struct LinkPanel *, s32);

void RegistryF246_ShowSlotReadyFrame(struct LinkPanel *, s32);

void RegistryF246_ShowSlotDefaultFrame(struct LinkPanel *, s32);

s32 Registry1745_GetSessionState(void);

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
            if (panel->entries[i].animationTimer > 30) {
                if (panel->entries[i].currentStatus > 1)
                    LinkPanel_SelectActiveAnimation(panel, i);
                else if (panel->entries[i].currentStatus == 1)
                    RegistryF246_ShowSlotReadyFrame(panel, i);
                else
                    goto inactive;
            } else
                goto increment;
        } else {
        inactive:
            RegistryF246_ShowSlotDefaultFrame(panel, i);
        }
        goto step;
    increment:
        panel->entries[i].animationTimer++;
    step:
        if (panel->selected[i] != panel->previous[i]) {
            Anim_SelectWidgetGlyph(&panel->animations[i], &panel->entryResource,
                                   panel->selected[i]);
            panel->previous[i] = panel->selected[i];
        }
        Anim_Step(&panel->animations[i], &panel->entryResource);
    }
}

void LinkPanel_UpdateLastEntry(struct LinkPanel *panel)
{
    if (panel->entries[0].currentStatus == 3)
        panel->selected[6] = 0x8E;
    else
        panel->selected[6] = 0x7F;
    if (panel->selected[6] != panel->previous[6]) {
        Anim_SelectWidgetGlyph(&panel->animations[6], &panel->lastEntryResource,
                               panel->selected[6]);
        panel->previous[6] = panel->selected[6];
    }
}

void LinkPanel_InitMessageDisplay(void)
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
    state = Registry1745_GetSessionState();
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
    state = Registry1745_GetSessionState();
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
