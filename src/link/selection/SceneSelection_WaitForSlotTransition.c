#include "global.h"
#include "engine/video/render.h"
#include "scene/areas/area_effect_link.h"
#include "engine/ram.h"
#include "engine/engine_pause_flags.h"

u8 LinkSession_ConsumeStateEntry(struct AreaEffectLinkState *);
s32 LinkSession_CheckSessionReady(struct AreaEffectLinkState *);
void LinkSession_BeginShutdown(struct AreaEffectLinkState *, void (*)(void));
void ScriptPanel_OpenTextWindow(void);
void Sound_FadeOutChannel10(u32);
void LinkSession_SetState(struct AreaEffectLinkState *, u32, void (*)(void));
void SessionMenu_CreateStageIconDisplay(void);
static inline u16 GetSlot(struct AreaEffectLinkState *p, s32 i)
{
    if (i < 0)
        return 0xffff;
    return p->selection[i];
}
static inline u8 IsTransitionSlot(u32 value)
{
    if (value == 0x2000)
        return 1;
    return 0;
}
s32 SceneSelection_WaitForSlotTransition(struct AreaEffectLinkState *p)
{
    s32 count, i, length;
    gLinkMenuActive = 1;
    if (LinkSession_ConsumeStateEntry(p)) {
        gBgFadeLevel = 0x40;
        gObjFadeLevel = 0x40;
        Sound_FadeOutChannel10(1);
    }
    if (p->menuElapsed == 1)
        Engine_SetPauseFlags(4);
    if (LinkSession_CheckSessionReady(p) < 0) {
        LinkSession_BeginShutdown(p, ScriptPanel_OpenTextWindow);
        return -1;
    }
    count = 0;
    i = 0;
    length = p->participantCount;
    if (i < length) {
        unsigned short mask = 0x3c00;
        do {
            if (IsTransitionSlot(GetSlot(p, i) & mask))
                count++;
            i++;
        } while (i < length);
    }
    if (count == length)
        LinkSession_SetState(p, 16, SessionMenu_CreateStageIconDisplay);
    else if (p->menuElapsed > 9) {
        p->requestedMode = 0x2000;
        Engine_ClearPauseFlags(4);
    }
    return 0;
}
