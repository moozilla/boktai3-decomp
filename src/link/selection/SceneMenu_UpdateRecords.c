#include "global.h"
#include "scene/areas/area_effect_link.h"
struct SceneMenuRecord {
    u8 data[0x3E];
    u16 checksum;
};
u8 LinkSession_ConsumeStateEntry(struct AreaEffectLinkState *);
void LinkSession_ClearParticipantCounters(struct AreaEffectLinkState *);
void ClockPanel_CaptureTime(struct AreaEffectLinkState *);
s32 LinkSession_CheckSessionReady(struct AreaEffectLinkState *);
void LinkSession_BeginShutdown(struct AreaEffectLinkState *, void (*)(void));
void LinkSession_SetState(struct AreaEffectLinkState *, u32, void (*)(void));
void ScriptPanel_OpenTextWindow(void);
void LinkMenu_UpdateSelection(void);
void LinkMenu_WaitSelectionMode(void);
u32 Crc_ComputeCcitt16(void *, s32, s32);
void ByteSequence_AdvanceEncodedValue(struct AreaEffectLinkState *, void *, s32);
u8 LinkSession_ReceiveSelectionByte(struct AreaEffectLinkState *, void *, s32, s32);
static inline u8 HasRecordInput(struct AreaEffectLinkState *p)
{
    u32 value = *p->selection;
    if ((value & 0x3C00) == 0x1800)
        return 1;
    return 0;
}
s32 SceneMenu_UpdateRecords(struct AreaEffectLinkState *p)
{
    u8 *record;
    s32 completed;
    s32 i;
    struct SceneMenuRecord *records = (struct SceneMenuRecord *)p->sessionData;
    if (LinkSession_ConsumeStateEntry(p)) {
        LinkSession_ClearParticipantCounters(p);
        ClockPanel_CaptureTime(p);
        records->checksum = Crc_ComputeCcitt16(records, 0, 0x3E);
    }
    if (LinkSession_CheckSessionReady(p) < 0) {
        LinkSession_BeginShutdown(p, ScriptPanel_OpenTextWindow);
        return -1;
    }
    if (HasRecordInput(p)) {
        if (gLinkSession && gLinkSession->localPlayer == 0) {
            LinkSession_SetState(p, 7, LinkMenu_UpdateSelection);
        } else {
            LinkSession_SetState(p, 8, LinkMenu_WaitSelectionMode);
        }
        return 0;
    }
    record = (u8 *)records;
    ByteSequence_AdvanceEncodedValue(p, record, 0x40);
    completed = 0;
    for (i = 0; i < p->participantCount; i++) {
        if (LinkSession_ReceiveSelectionByte(p, &records[i + 1], i, 0x40) == 1) {
            Crc_ComputeCcitt16(&records[i + 1], 0, 0x3E);
            completed++;
        }
    }
    if (completed == p->participantCount && gLinkSession && gLinkSession->localPlayer == 0) {
        p->requestedMode = 0x1800;
    }
}
