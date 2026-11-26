#include "global.h"
#include "link/selection/link_selection_mode.h"
#include "scene/areas/area_effect_link.h"

// foundry draft (worker best 96.4%); what differs is mostly register allocation and instruction order
u8 Registry1745_ConsumeStateEntry(struct AreaEffectLinkState *);
s32 Registry1745_CheckSessionReady(struct AreaEffectLinkState *);
void Registry1745_BeginShutdown(struct AreaEffectLinkState *, void (*)(void));
void ScriptPanel_OpenTextWindow(void);
void Registry1745_SetState(struct AreaEffectLinkState *, u32, void (*)(void));
void SceneMenu_BeginTimedMessage(void);
void SessionMenu_ShowTimedMessage(void);
void Sound_StopAllTrackedSongs(void);
void Hud_SetMode(s32);
void Script_Call(u32, void *);
static inline u16 GetSlot(struct AreaEffectLinkState *p, s32 i)
{
    if (i < 0)
        return -1;
    return p->selection[i];
}
s32 LinkSelection_WaitForParticipantChoices(struct AreaEffectLinkState *p)
{
    s32 count, selected, i, length;
    if (Registry1745_ConsumeStateEntry(p)) {
        Sound_StopAllTrackedSongs();
        Hud_SetMode(7);
        Script_Call(p->readyScript, 0);
        p->menuChoice = -1;
    }
    if (Registry1745_CheckSessionReady(p) < 0) {
        Registry1745_BeginShutdown(p, ScriptPanel_OpenTextWindow);
        return -1;
    }
    count = 0;
    selected = 0;
    for (i = 0, length = p->participantCount; i < length; i++) {
        u16 slot = GetSlot(p, i);
        if (LinkSelection_IsChoice(slot, 0x3c00)) {
            if (slot & 0x3f)
                selected++;
            count++;
        }
    }
    if (count == length) {
        if (count == selected)
            Registry1745_SetState(p, 21, SceneMenu_BeginTimedMessage);
        else
            Registry1745_SetState(p, 23, SessionMenu_ShowTimedMessage);
    } else if (p->menuChoice >= 0) {
        u32 choice = p->menuChoice;
        u32 shifted = choice << 16;
        u32 k = 0x2c000000;
        k |= shifted;
        p->requestedMode = k << 2 >> 18;
    }
    return 0;
}
