#define gLinkMenuFlags gEnginePauseFlags
#include "link/selection/link_selection_mode.h"
#include "global.h"
#include "scene/areas/area_effect_link.h"
#include "system/game_state/game_state.h"
#include "engine/video/render.h"
#include "engine/ram.h"
#include "engine/engine_pause_flags.h"
u8 LinkSession_ConsumeStateEntry(struct AreaEffectLinkState *);
s32 LinkSession_CheckSessionReady(struct AreaEffectLinkState *);
void LinkSession_BeginShutdown(struct AreaEffectLinkState *, void (*)(void));
void ScriptPanel_OpenTextWindow(void);
void LinkSession_SetState(struct AreaEffectLinkState *, u32, void (*)(void));
void LinkSelection_WaitForParticipantChoices(void);
void LinkSelection_UpdateParticipantScores(struct AreaEffectLinkState *);
void PaletteLevels_Reset(void);
void Sound_FadeOutChannel10(u32);
static inline u16 GetSlot(struct AreaEffectLinkState *p, s32 i)
{
    if (i < 0)
        return -1;
    return p->selection[i];
}
/* The waiting path preserves r0 from the validity check; no explicit return in the original. */
s32 LinkSelection_WaitForReadyParticipants(struct AreaEffectLinkState *p)
{
    struct AreaEffectLinkState *session = p;
    s32 count, i, length, result;
    if (LinkSession_ConsumeStateEntry(p)) {
        s16 *counter;
        u16 value;
        s32 increment;
        Engine_SetPauseFlags(4);
        PaletteLevels_Reset();
        Sound_FadeOutChannel10(6);
        gLinkMenuActive = 0;
        counter = &gGameState->unk7B4;
        value = *counter;
        if (*counter < 0)
            value = 0;
        *counter = value;
        counter = &gGameState->unk7B4;
        increment = *counter + 1;
        if (increment > 9999)
            increment = 9999;
        *counter = increment;
    }
    result = LinkSession_CheckSessionReady(p);
    if (result < 0) {
        LinkSession_BeginShutdown(p, ScriptPanel_OpenTextWindow);
        return -1;
    }
    count = 0;
    for (i = 0, length = p->participantCount; i < length; i++) {
        u16 slot = GetSlot(p, i);
        if (LinkSelection_IsReady(slot, 0x3c00) && (slot & 0x3f) == 0)
            count++;
    }
    if (count == length) {
        LinkSession_SetState(p, 20, LinkSelection_WaitForParticipantChoices);
        p->readyPhase = 0;
        LinkSelection_UpdateParticipantScores(session);
        return 0;
    }
    if (p->menuElapsed > 89)
        p->requestedMode = 0x1000;
}
