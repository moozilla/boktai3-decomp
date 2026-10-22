#include "global.h"
#include "area_effect_link.h"
u8 Registry1745_ConsumeStateEntry(void *);
void Registry1745_ClearParticipantCounters(void *);
s32 Registry1745_CheckSessionReady(void *);
void Registry1745_BeginShutdown(void *, void (*)(void));
void Registry1745_SetState(void *, u32, void (*)(void));
void ClockPanel_CaptureSnapshot(void *);
s32 Crc_ComputeCcitt16(void *, s32, s32);
u8 ByteSequence_AdvanceEncodedValue(void *, void *, s32);
u8 LinkSession_ReceiveSelectionByte(void *, void *, s32, s32);
void ScriptPanel_OpenTextWindow(void);
void SessionMenu_WaitForSelection(void);
static inline u8 IsSelectionReady(u32 value, u32 mask)
{
    if ((value & mask) == 0x1800)
        return 1;
    return 0;
}
s32 sub_0810A304(struct AreaEffectLinkState *p)
{
    u8 *session = p->sessionData;
    s32 count, i;
    u8 *entry;
    if (Registry1745_ConsumeStateEntry(p)) {
        Registry1745_ClearParticipantCounters(p);
        ClockPanel_CaptureSnapshot(p);
        *(u16 *)(session + 0x3e) = Crc_ComputeCcitt16(session, 0, 0x3e);
    }
    if (Registry1745_CheckSessionReady(p) < 0) {
        Registry1745_BeginShutdown(p, ScriptPanel_OpenTextWindow);
        return -1;
    }
    if (IsSelectionReady(*p->selection, 0x3c00)) {
        Registry1745_SetState(p, 6, SessionMenu_WaitForSelection);
        return 0;
    }
    ByteSequence_AdvanceEncodedValue(p, session, 0x40);
    count = 0;
    i = 0;
    if (count < p->participantCount) {
        entry = session + 0x40;
        do {
            if (LinkSession_ReceiveSelectionByte(p, entry, i, 0x40) == 1) {
                Crc_ComputeCcitt16(entry, 0, 0x3e);
                count++;
            }
            entry += 0x40;
            i++;
        } while (i < p->participantCount);
    }
    if (count == p->participantCount && p->exchangeBusy == 0)
        p->requestedMode = 0x1800;
    if (p->menuElapsed > 0xe10) {
        Registry1745_BeginShutdown(p, ScriptPanel_OpenTextWindow);
        return -1;
    }
    return 0;
}
