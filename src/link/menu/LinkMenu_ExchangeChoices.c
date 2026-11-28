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
/* The choice exchange's view of the session data. */
struct LinkChoiceData {
    u8 unknown0[0x60];
    u16 randomSeed; /* confidence=low */
    u8 unknown62[0x12C - 0x62];
    u8 outgoing[4]; /* the first player's value and its CRC */
    u8 incoming[4];
};
s32 LinkMenu_ExchangeChoices(struct AreaEffectLinkState *p)
{
    struct LinkChoiceData *data = (struct LinkChoiceData *)p->sessionData;

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
        ByteSequence_AdvanceEncodedValue(p, data->outgoing, 4);
    if (LinkSession_ReceiveSelectionByte(p, data->incoming, 0, 4) == 1) {
        Crc_ComputeCcitt16(data->incoming, 0, 2);
        SlotState_ResetInactiveMode(data);
        gRandomIndex = data->randomSeed & 0x3ff;
        LinkSession_SetState(p, 10, LinkMenu_ShowChoiceMessage);
    }
    return 0;
}
