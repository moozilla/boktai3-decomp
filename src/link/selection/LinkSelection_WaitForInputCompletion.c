#include "global.h"
#include "gba/io_reg.h"
#include "scene/areas/area_effect_link.h"
#include "engine/ram.h"
u8 LinkSession_ConsumeStateEntry(void *);
s32 LinkSession_CheckSessionReady(void *);
void LinkSession_BeginShutdown(void *, void *);
void ScriptPanel_OpenTextWindow(void);
void StageIconDisplay_Create(s32, s32, void *, void *);
void LinkSelection_RecordSlotState(void *, u16, s32, u32 *, s32);
void LinkSelection_AdvanceBitExchange(void *, u32, u16 *);
void LinkSession_SetState(void *, u32, void *);
void LinkSelection_WaitForReadyParticipants(void);
struct ExchangeCounters {
    u8 pad0[0x184];
    u32 frames;
};
static inline u16 Slot(struct AreaEffectLinkState *p, s32 i)
{
    if (i < 0)
        return -1;
    return p->selection[i];
}
static inline u8 ModeLeave(u32 value)
{
    if ((value & 0x3c00) == 0x3400)
        return 1;
    return 0;
}
static inline u8 ModeInput(u32 value)
{
    if ((value & 0x3c00) == 0xc00)
        return 1;
    return 0;
}
static inline u8 ModeCancel(u32 value)
{
    if ((value & 0x3c00) == 0x2800)
        return 1;
    return 0;
}
static inline u16 PackKeys(u32 value, u32 mode)
{
    value |= mode;
    value &= 0x3fff;
    return value;
}
s32 LinkSelection_WaitForInputCompletion(struct AreaEffectLinkState *p)
{
    struct ExchangeCounters *c = (void *)p->sessionData;
    u16 keys;
    u32 flags;
    s32 ready, i;
    gLinkMenuActive = 1;
    if (LinkSession_ConsumeStateEntry(p)) {
        StageIconDisplay_Create(1, 0, 0, p);
    }
    if (LinkSession_CheckSessionReady(p) < 0) {
        LinkSession_BeginShutdown(p, ScriptPanel_OpenTextWindow);
        return -1;
    }
    keys = REG_KEYINPUT;
    keys &= 0xfff3;
    ready = 0;
    flags = 0;
    for (i = 0; i < p->participantCount; ++i) {
        u16 value = Slot(p, i);
        if (ModeLeave(value)) {
            LinkSelection_RecordSlotState(p, value, i, &flags, 0);
            ready++;
        } else if (ModeInput(value) || ModeCancel(value)) {
            LinkSelection_RecordSlotState(p, value, i, &flags, 0);
            p->menuElapsed = 0;
        } else if (value == 0xffff) {
            LinkSession_BeginShutdown(p, ScriptPanel_OpenTextWindow);
            return -1;
        }
    }
    if (p->menuElapsed > 60 && ready >= p->participantCount) {
        LinkSession_SetState(p, 19, LinkSelection_WaitForReadyParticipants);
    } else {
        if (!p->menuElapsed)
            LinkSelection_AdvanceBitExchange(p, flags, &keys);
        p->requestedMode = PackKeys(keys, 0x3400);
        c->frames++;
    }
    return 0;
}
