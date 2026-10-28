#define LinkMenu_ConsumeStateEntry Registry1745_ConsumeStateEntry
#define LinkMenu_SetState          Registry1745_SetState
#define LinkMenu_CheckSession      Registry1745_CheckSessionReady
#define LinkMenu_ResetSelection    Registry1745_BeginShutdown
#include "global.h"
#include "area_effect_link.h"
u8 Registry1745_ConsumeStateEntry(void *);
s32 Registry1745_CheckSessionReady(void *);
void Registry1745_BeginShutdown(void *, void (*)(void));
void Registry1745_SetState(void *, u32, void (*)(void));
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
static inline u8 IsChoiceMode(u32 value, u32 mask)
{
    value &= mask;
    if (value == 0x1000)
        return 1;
    return 0;
}
s32 LinkMenu_ShowChoiceMessage(struct AreaEffectLinkState *p)
{
    u8 *session = p->sessionData;
    s32 count, i, limit;
    u32 mask;
    if (Registry1745_ConsumeStateEntry(p)) {
        u8 *values;
        s32 *window;
        Hud_RefreshSpriteFromScript();
        Hud_SetMode(5);
        values = p->incomingChoices;
        SlotText_CopySelectedString(values, session);
        p->menuTextWindow = TextWindow_Create(1, 6, 28, 6);
        window = &p->menuTextWindow;
        TextWindow_SetScript(*window, p->choiceMessageScript);
        TextWindow_Refresh(*window, 0);
        TextWindow_SetString(*window, 1, values);
        TextWindow_StartPlayback(*window);
    }
    if (Registry1745_CheckSessionReady(p) < 0) {
        Registry1745_BeginShutdown(p, ScriptPanel_OpenTextWindow);
        return -1;
    }
    count = 0;
    i = 0;
    limit = p->participantCount;
    if (i < limit) {
        mask = 0x3c00;
        do {
            u16 value = SelectionAt(p, i);
            if (IsChoiceMode(value, mask) && (value & 0xff) == 0x8b)
                count++;
        } while (++i < limit);
    }
    if (count == limit)
        Registry1745_SetState(p, 11, SessionMenu_RunEventScript);
    else if (p->menuElapsed > 120)
        p->requestedMode = 0x108b;
    return 0;
}
