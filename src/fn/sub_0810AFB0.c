#include "global.h"
#include "area_effect_link.h"
#include "ram.h"
u8 Registry1745_ConsumeStateEntry(void *);
s32 Registry1745_CheckSessionReady(void *);
void Registry1745_BeginShutdown(void *, void *);
void ScriptPanel_OpenTextWindow(void);
void StageIconDisplay_Create(s32, s32, void *, void *);
void LinkSelection_RecordSlotState(void *, u16, s32, u32 *, s32);
void LinkSelection_AdvanceBitExchange(void *, u32, u16 *);
void Registry1745_SetState(void *, u32, void *);
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
static inline u8 ModeLeave(u32 input, u32 mask)
{
    u32 value = input;
    u32 maskCopy;
    maskCopy = mask;
    value &= maskCopy;
    if (value == 0x3400)
        return 1;
    return 0;
}
static inline u8 ModeInput(u32 input, u32 mask)
{
    u32 value = input;
    u32 maskCopy;
    maskCopy = mask;
    value &= maskCopy;
    if (value == 0xc00)
        return 1;
    return 0;
}
static inline u8 ModeCancel(u32 input, u32 mask)
{
    u32 value = input;
    u32 maskCopy;
    maskCopy = mask;
    value &= maskCopy;
    if (value == 0x2800)
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
    if (Registry1745_ConsumeStateEntry(p)) {
        StageIconDisplay_Create(1, 0, 0, p);
    }
    if (Registry1745_CheckSessionReady(p) < 0) {
        goto shutdown;
    }
    keys = *(vu16 *)0x04000130;
    keys &= 0xfff3;
    ready = 0;
    flags = ready;
    i = 0;
    if (ready < p->participantCount) {
        u32 initialMask = 0x3c00;
        u32 mask;
        *(&mask) = initialMask;
        do {
            u16 value = Slot(p, i);
            if (ModeLeave(value, mask)) {
                LinkSelection_RecordSlotState(p, value, i, &flags, 0);
                ready++;
            } else if (ModeInput(value, mask) || ModeCancel(value, mask)) {
                LinkSelection_RecordSlotState(p, value, i, &flags, 0);
                p->menuElapsed = 0;
            } else if (value == 0xffff) {
                goto shutdown;
            }
        } while (++i < p->participantCount);
    }
    if (p->menuElapsed > 60 && ready >= p->participantCount) {
        Registry1745_SetState(p, 19, LinkSelection_WaitForReadyParticipants);
        goto end;
    }
    goto waiting;
shutdown:
    Registry1745_BeginShutdown(p, ScriptPanel_OpenTextWindow);
    return -1;
waiting:
    if (!p->menuElapsed) {
        LinkSelection_AdvanceBitExchange(p, flags, &keys);
    }

    p->requestedMode = PackKeys(keys, 0x3400);
    c->frames++;
end:
    return 0;
}
