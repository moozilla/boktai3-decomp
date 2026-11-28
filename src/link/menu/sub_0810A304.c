#include "global.h"
#include "link/selection/link_selection_mode.h"
#include "scene/areas/area_effect_link.h"

/* Session data is exchanged in 0x40-byte records: ours first, then one per participant. */
struct LinkExchangeRecord {
    u8 data[0x3E];
    u16 crc;
};

u8 LinkSession_ConsumeStateEntry(void *);
void LinkSession_ClearParticipantCounters(void *);
s32 LinkSession_CheckSessionReady(void *);
void LinkSession_BeginShutdown(void *, void (*)(void));
void LinkSession_SetState(void *, u32, void (*)(void));
void ClockPanel_CaptureSnapshot(void *);
s32 Crc_ComputeCcitt16(void *, s32, s32);
u8 ByteSequence_AdvanceEncodedValue(void *, void *, s32);
u8 LinkSession_ReceiveSelectionByte(void *, void *, s32, s32);
void ScriptPanel_OpenTextWindow(void);
void SessionMenu_WaitForSelection(void);
s32 sub_0810A304(struct AreaEffectLinkState *p)
{
    struct LinkExchangeRecord *records = (struct LinkExchangeRecord *)p->sessionData;
    s32 count, i;

    if (LinkSession_ConsumeStateEntry(p)) {
        LinkSession_ClearParticipantCounters(p);
        ClockPanel_CaptureSnapshot(p);
        records[0].crc = Crc_ComputeCcitt16(records, 0, 0x3e);
    }
    if (LinkSession_CheckSessionReady(p) < 0) {
        LinkSession_BeginShutdown(p, ScriptPanel_OpenTextWindow);
        return -1;
    }
    if (LinkSelection_IsSelected(*p->selection, 0x3c00)) {
        LinkSession_SetState(p, 6, SessionMenu_WaitForSelection);
        return 0;
    }
    ByteSequence_AdvanceEncodedValue(p, records, 0x40);
    count = 0;
    for (i = 0; i < p->participantCount; i++) {
        if (LinkSession_ReceiveSelectionByte(p, &records[i + 1], i, 0x40) == 1) {
            Crc_ComputeCcitt16(&records[i + 1], 0, 0x3e);
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
