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
/* The payload exchange's view of the session data. */
struct LinkPayloadData {
    u8 unknown0[0xC0];
    u8 outgoing[0x24]; /* payload and its CRC */
    u8 incoming[4][0x24];
};
static inline u8 IsPayloadMode(u16 value)
{
    value &= 0x3c00;
    if (value == 0x1800)
        return 1;
    return 0;
}
s32 LinkMenu_ExchangePayload(struct AreaEffectLinkState *p)
{
    struct LinkPayloadData *data = (struct LinkPayloadData *)p->sessionData;
    s32 count, i;

    if (LinkSession_ConsumeStateEntry(p)) {
        LinkSession_ClearParticipantCounters(p);
        SceneActorStyle_InitializeFromGameState(p->outgoingPayload);
        p->outgoingChecksum = Crc_ComputeCcitt16(p->outgoingPayload, 0, 0x20);
    }
    if (LinkSession_CheckSessionReady(p) < 0) {
        LinkSession_BeginShutdown(p, ScriptPanel_OpenTextWindow);
        return -1;
    }
    if (IsPayloadMode(*p->selection)) {
        LinkSession_SetState(p, 14, LinkSelection_WaitForEmptySlots);
        return 0;
    }
    ByteSequence_AdvanceEncodedValue(p, data->outgoing, 0x24);
    count = 0;
    for (i = 0; i < p->participantCount; i++) {
        if (LinkSession_ReceiveSelectionByte(p, data->incoming[i], i, 0x24) == 1) {
            Crc_ComputeCcitt16(data->incoming[i], 0, 0x20);
            count++;
        }
    }
    if (count == p->participantCount && !p->exchangeBusy)
        p->requestedMode = 0x1800;
    if (p->menuElapsed > 3600) {
        LinkSession_BeginShutdown(p, ScriptPanel_OpenTextWindow);
        return -1;
    }
    return 0;
}
