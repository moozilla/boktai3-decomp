#define LinkMenu_ConsumeStateEntry     LinkSession_ConsumeStateEntry
#define LinkMenu_SetState              LinkSession_SetState
#define LinkMenu_CheckSession          LinkSession_CheckSessionReady
#define LinkMenu_ResetSelection        LinkSession_BeginShutdown
#define LinkMenu_ClearExchangeCounters LinkSession_ClearParticipantCounters
#include "global.h"
#include "scene/areas/area_effect_link.h"
u8 LinkSession_ConsumeStateEntry(void *);
void LinkSession_ClearParticipantCounters(void *);
s32 LinkSession_CheckSessionReady(void *);
void LinkSession_BeginShutdown(void *, void (*)(void));
void LinkSession_SetState(void *, u32, void (*)(void));
s32 Crc_ComputeCcitt16(void *, s32, s32);
u8 ByteSequence_AdvanceEncodedValue(void *, void *, s32);
u8 LinkSession_ReceiveSelectionByte(void *, void *, s32, s32);
void SceneActorStyle_InitializeFromGameState(void *);
void ScriptPanel_OpenTextWindow(void);
void LinkSelection_WaitForEmptySlots(void);
static inline u8 IsPayloadMode(u16 value)
{
    value &= 0x3c00;
    if (value == 0x1800)
        return 1;
    return 0;
}
s32 LinkMenu_ExchangePayload(struct AreaEffectLinkState *p)
{
    u8 *session = p->sessionData;
    s32 count, i;
    u8 *incoming;
    if (LinkSession_ConsumeStateEntry(p)) {
        u8 *outgoing;
        LinkSession_ClearParticipantCounters(p);
        outgoing = p->outgoingPayload;
        SceneActorStyle_InitializeFromGameState(outgoing);
        p->outgoingChecksum = Crc_ComputeCcitt16(outgoing, 0, 0x20);
    }
    if (LinkSession_CheckSessionReady(p) < 0)
        goto fail;
    if (IsPayloadMode(*p->selection)) {
        LinkSession_SetState(p, 14, LinkSelection_WaitForEmptySlots);
        return 0;
    }
    ByteSequence_AdvanceEncodedValue(p, session + 0xc0, 0x24);
    count = 0;
    i = 0;
    if (i < p->participantCount) {
        incoming = session + 0xe4;
        do {
            if (LinkSession_ReceiveSelectionByte(p, incoming, i, 0x24) == 1) {
                Crc_ComputeCcitt16(incoming, 0, 0x20);
                count++;
            }
            incoming += 0x24;
        } while (++i < p->participantCount);
    }
    if (count == p->participantCount && !p->exchangeBusy)
        p->requestedMode = 0x1800;
    if (p->menuElapsed <= 3600)
        return 0;
fail:
    LinkSession_BeginShutdown(p, ScriptPanel_OpenTextWindow);
    return -1;
}
