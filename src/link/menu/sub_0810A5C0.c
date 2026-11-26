#define LinkMenu_ConsumeStateEntry     LinkSession_ConsumeStateEntry
#define LinkMenu_SetState              LinkSession_SetState
#define LinkMenu_CheckSession          LinkSession_CheckSessionReady
#define LinkMenu_ResetSelection        LinkSession_BeginShutdown
#define LinkMenu_ClearExchangeCounters LinkSession_ClearParticipantCounters
#include "global.h"
#include "scene/areas/area_effect_link.h"
#include "engine/engine.h"
u8 LinkSession_ConsumeStateEntry(void *);
void LinkSession_ClearParticipantCounters(void *);
s32 LinkSession_CheckSessionReady(void *);
void LinkSession_BeginShutdown(void *, void (*)(void));
void LinkSession_SetState(void *, u32, void (*)(void));
s32 Crc_ComputeCcitt16(void *, s32, s32);
u8 ByteSequence_AdvanceEncodedValue(void *, void *, s32);
u8 LinkSession_ReceiveSelectionByte(void *, void *, s32, s32);
void SlotState_ResetInactiveMode(void *);
void ScriptPanel_OpenTextWindow(void);
void LinkMenu_ShowChoiceMessage(void);
s32 LinkMenu_ExchangeChoices(struct AreaEffectLinkState *p)
{
    u8 *session = p->sessionData;
    u8 *incoming;
    if (LinkSession_ConsumeStateEntry(p)) {
        LinkSession_ClearParticipantCounters(p);
        if (gLinkSession && gLinkSession->localPlayer == 0)
            p->exchangeText = Crc_ComputeCcitt16(&p->exchangeValue, 0, 2);
    }
    if (LinkSession_CheckSessionReady(p) < 0) {
        LinkSession_BeginShutdown(p, ScriptPanel_OpenTextWindow);
        return -1;
    }
    if (gLinkSession && gLinkSession->localPlayer == 0)
        ByteSequence_AdvanceEncodedValue(p, session + 0x12c, 4);
    incoming = session + 0x130;
    if (LinkSession_ReceiveSelectionByte(p, incoming, 0, 4) == 1) {
        Crc_ComputeCcitt16(incoming, 0, 2);
        SlotState_ResetInactiveMode(session);
        gRandomIndex = *(u16 *)(session + 0x60) & 0x3ff;
        LinkSession_SetState(p, 10, LinkMenu_ShowChoiceMessage);
    }
    return 0;
}
