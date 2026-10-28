#include "global.h"
#include "area_effect_link.h"
struct SceneMenuRecord {
    u8 data[0x3E];
    u16 checksum;
};
struct SceneMenu {
    u8 pad0[0x1A];
    u16 count;
    u8 pad1C[0x22];
    u16 flags;
    u16 *input;
    u8 pad44[0x9C];
    struct SceneMenuRecord records[1];
};
struct SceneMenuPeer {
    u8 pad0[0x18];
    s16 state;
};
u8 Registry1745_ConsumeStateEntry(struct SceneMenu *);
void Registry1745_ClearParticipantCounters(struct SceneMenu *);
void ClockPanel_CaptureTime(struct SceneMenu *);
s32 Registry1745_CheckSessionReady(struct SceneMenu *);
void Registry1745_BeginShutdown(struct SceneMenu *, void (*)(void));
void Registry1745_SetState(struct SceneMenu *, u32, void (*)(void));
void ScriptPanel_OpenTextWindow(void);
void LinkMenu_UpdateSelection(void);
void LinkMenu_WaitSelectionMode(void);
u32 Crc_ComputeCcitt16(void *, s32, s32);
void ByteSequence_AdvanceEncodedValue(struct SceneMenu *, void *, s32);
u8 LinkSession_ReceiveSelectionByte(struct SceneMenu *, void *, s32, s32);
static inline u8 HasRecordInput(struct SceneMenu *p)
{
    u32 value = *p->input;
    if ((value & 0x3C00) == 0x1800)
        return 1;
    return 0;
}
s32 SceneMenu_UpdateRecords(struct SceneMenu *object)
{
    u8 *record;
    struct SceneMenu *p = object;
    s32 completed;
    s32 i;
    do {
        record = (u8 *)p->records;
        if (Registry1745_ConsumeStateEntry(p)) {
            Registry1745_ClearParticipantCounters(p);
            ClockPanel_CaptureTime(p);
            *(u16 *)(record + 0x3E) = Crc_ComputeCcitt16(record, 0, 0x3E);
        }
    } while (0);
    if (Registry1745_CheckSessionReady(p) < 0) {
        Registry1745_BeginShutdown(p, ScriptPanel_OpenTextWindow);
        return -1;
    }
    if (HasRecordInput(p)) {
        if ((*(struct SceneMenuPeer **)&gLinkSession) &&
            ((*(struct SceneMenuPeer **)&gLinkSession)->state == 0)) {
            Registry1745_SetState(p, 7, LinkMenu_UpdateSelection);
        } else {
            Registry1745_SetState(p, 8, LinkMenu_WaitSelectionMode);
        }
        return 0;
    }
    ByteSequence_AdvanceEncodedValue(p, record, 0x40);
    completed = 0;
    i = 0;
    if (completed < p->count) {
        record += 0x40;
        do {
            if (LinkSession_ReceiveSelectionByte(p, record, i, 0x40) == 1) {
                Crc_ComputeCcitt16(record, 0, 0x3E);
                completed++;
            }
            record += 0x40;
            i++;
        } while (i < p->count);
    }
    if (((completed == p->count) && (*(struct SceneMenuPeer **)&gLinkSession)) &&
        ((*(struct SceneMenuPeer **)&gLinkSession)->state == 0)) {
        p->flags = 0x1800;
    }
}
