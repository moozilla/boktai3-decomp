#define ScriptPanel_CompareSnapshots sub_0810A304
#define ClockPanel_CaptureSnapshot   sub_08109AEC
#define ScriptPanel_OpenTextWindow   sub_08109C80
#include "global.h"
struct PanelSnapshot {
    u8 bytes[0x3E];
    u16 checksum;
};
struct ScriptPanel {
    u8 unk0[0x1A];
    u16 count;
    u8 unk1C;
    u8 mode;
    u8 unk1E[0x12];
    u32 counter;
    u8 unk34[0xA];
    u16 flags;
    u16 *status;
    u8 unk44[0x9C];
    struct PanelSnapshot snapshot;
};
u8 sub_08138168(struct ScriptPanel *);
void sub_08138184(struct ScriptPanel *);
void sub_08109AEC(struct ScriptPanel *);
s32 sub_08221594(void *, s32, s32);
s32 sub_08138288(struct ScriptPanel *);
void sub_08138154(struct ScriptPanel *, u32, void *);
void sub_0810A3E0(void);
void sub_08109C80(void);
void sub_081382BC(struct ScriptPanel *, void *, u32);
u8 sub_08138300(struct ScriptPanel *, void *, u32, u32);
void sub_08138380(struct ScriptPanel *, void *);
static inline u8 ScriptPanel_HasReadyStatus(struct ScriptPanel *p)
{
    u32 status = *p->status;
    if ((status & 0x3C00) == 0x1800)
        return TRUE;
    return FALSE;
}
s32 sub_0810A304(struct ScriptPanel *input)
{
    struct PanelSnapshot *snapshot = &input->snapshot;
    struct ScriptPanel *p = input;
    if (sub_08138168(p)) {
        sub_08138184(p);
        sub_08109AEC(p);
        snapshot->checksum = sub_08221594(snapshot, 0, 0x3E);
    }
    if (sub_08138288(p) >= 0) {
        s32 matches, i;
        if (ScriptPanel_HasReadyStatus(p)) {
            sub_08138154(p, 6, sub_0810A3E0);
            return 0;
        }
        sub_081382BC(p, snapshot, 0x40);
        matches = 0;
        i = 0;
        if (i < p->count) {
            snapshot++;
            do {
                if (sub_08138300(p, snapshot, i, 0x40) == 1) {
                    sub_08221594(snapshot, 0, 0x3E);
                    matches++;
                }
                snapshot++;
                i++;
            } while (i < p->count);
        }
        if (matches == p->count && !p->mode)
            p->flags = 0x1800;
        if (p->counter <= 0xE10)
            return 0;
    }
    sub_08138380(p, sub_08109C80);
    return -1;
}
