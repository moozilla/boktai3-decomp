#include "global.h"
#include "scene/areas/area_effect_link.h"
#include "link/selection/link_selection_mode.h"
u8 LinkSession_ConsumeStateEntry(void *);
s32 LinkSession_CheckSessionReady(void *);
void LinkSession_BeginShutdown(void *, void (*)(void));
void LinkSession_SetState(void *, u32, void (*)(void));
void ScriptPanel_OpenTextWindow(void);
void SessionMenu_RunEventScript(void);
void Hud_RefreshSpriteFromScript(void);
void Hud_SetMode(s32);
s32 TextWindow_Create(s32, s32, s32, s32);
s32 TextWindow_SetScript(s32, const u8 *);
s32 TextWindow_Refresh(s32, s32);
void TextWindow_SetString(s32, s32, void *);
void TextWindow_StartPlayback(s32);
void SlotText_CopySelectedString(void *, void *);
static inline u16 SelectionAt(struct AreaEffectLinkState *p, s32 index)
{
    if (index < 0)
        return 0xffff;
    return p->selection[index];
}
s32 LinkMenu_ShowChoiceMessage(struct AreaEffectLinkState *p)
{
    u8 *session = p->sessionData;
    s32 count, i;

    if (LinkSession_ConsumeStateEntry(p)) {
        Hud_RefreshSpriteFromScript();
        Hud_SetMode(5);
        SlotText_CopySelectedString(p->incomingChoices, session);
        p->menuTextWindow = TextWindow_Create(1, 6, 28, 6);
        TextWindow_SetScript(p->menuTextWindow, p->choiceMessageScript);
        TextWindow_Refresh(p->menuTextWindow, 0);
        TextWindow_SetString(p->menuTextWindow, 1, p->incomingChoices);
        TextWindow_StartPlayback(p->menuTextWindow);
    }
    if (LinkSession_CheckSessionReady(p) < 0) {
        LinkSession_BeginShutdown(p, ScriptPanel_OpenTextWindow);
        return -1;
    }
    count = 0;
    for (i = 0; i < p->participantCount; i++) {
        u16 value = SelectionAt(p, i);
        if (LinkSelection_IsReady(value, 0x3c00) && (value & 0xff) == 0x8b)
            count++;
    }
    if (count == p->participantCount)
        LinkSession_SetState(p, 11, SessionMenu_RunEventScript);
    else if (p->menuElapsed > 120)
        p->requestedMode = 0x108b;
    return 0;
}
