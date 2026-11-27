#include "global.h"
#include "scene/areas/area_effect_link.h"
#include "link/selection/link_selection_prompt.h"
#define SceneMenu_SetState LinkSession_SetState
u8 LinkSession_ConsumeStateEntry(struct LinkSelectionPrompt *);
s32 LinkSession_CheckSessionReady(struct LinkSelectionPrompt *);
void LinkSession_BeginShutdown(struct LinkSelectionPrompt *, void (*)(void));
void LinkSession_SetState(struct LinkSelectionPrompt *, u32, void (*)(void));
void ScriptPanel_OpenTextWindow(void);
void SceneMenu_UpdateRecords(void);
s32 TextWindow_Create(s32, s32, s32, s32);
s32 TextWindow_SetScript(s32, const u8 *);
s32 TextWindow_Refresh(s32, s32);
void TextWindow_StartPlayback(s32);
s32 SceneMenu_BeginTimedMessage(struct LinkSelectionPrompt *p)
{
    if (LinkSession_ConsumeStateEntry(p)) {
        p->field260 = 0;
        p->field264 = 0;
        if (((u8 *)gLinkSession))
            ((u8 *)gLinkSession)[0x52] = 1;
        p->window = TextWindow_Create(1, 6, 28, 6);
        TextWindow_SetScript(p->window, p->script);
        TextWindow_Refresh(p->window, 8);
        TextWindow_StartPlayback(p->window);
    }
    if (LinkSession_CheckSessionReady(p) < 0) {
        LinkSession_BeginShutdown(p, ScriptPanel_OpenTextWindow);
        return -1;
    }
    if (p->timer <= 59)
        LinkSession_SetState(p, 0x16, SceneMenu_UpdateRecords);
    return 0;
}
