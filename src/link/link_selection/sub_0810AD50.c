#include "engine/shared/engine_pause.h"
#include "engine/shared/global.h"
#include "engine/shared/area_effect_link.h"
#include "system/game_state/game_state.h"
#include "engine/shared/engine.h"
#include "engine/shared/ram.h"
#include "engine/shared/scene_pool.h"
u8 Registry1745_ConsumeStateEntry(void *);
s32 Registry1745_CheckSessionReady(void *);
void Registry1745_BeginShutdown(void *, void *);
void ScriptPanel_OpenTextWindow(void);
void LinkSelection_RecordSlotState(void *, u32, s32, u32 *, s32);
void LinkSelection_AdvanceBitExchange(void *, u32, u16 *);
void Registry1745_SetState(void *, u32, void *);
void LinkSelection_WaitForInputCompletion(void);
struct ExchangeCounters {
    u8 pad0[0x180];
    u32 elapsed;
    u32 frames;
};
static inline u16 Slot(struct AreaEffectLinkState *p, s32 i)
{
    if (i < 0)
        return -1;
    return p->selection[i];
}
static inline u8 ModeInput(u16 value)
{
    u16 mode = value & 0x3c00;
    if (mode == 0xc00)
        return 1;
    return 0;
}
static inline u8 ModeLeave(u16 value)
{
    u16 mode = value & 0x3c00;
    if (mode == 0x3400)
        return 1;
    return 0;
}
static inline u8 ModeCancel(u16 value)
{
    u16 mode = value & 0x3c00;
    if (mode == 0x2800)
        return 1;
    return 0;
}
static inline u16 PackKeys(u32 value, u32 mode)
{
    value |= mode;
    value &= 0x3fff;
    return value;
}
static inline s32 SessionState(void)
{
    if (!gLinkSession)
        return -1;
    return gLinkSession->unknown1C;
}
s32 LinkSelection_ExchangeParticipantInput(struct AreaEffectLinkState *p)
{
    struct ExchangeCounters *c = (void *)p->sessionData;
    u16 keys;
    u32 flags;
    s32 i;
    gLinkMenuActive = 1;
    if (Registry1745_ConsumeStateEntry(p)) {
        p->receiveBits = (u16)gGameState->unk874 >> 2;
        p->sendBit = 0;
        p->receiveBit = 0;
        for (i = 0; i < p->participantCount; i++)
            p->selectionBits[i + p->selectionPage * 4] = 0;
        c->elapsed = 0;
        c->frames = 0;
    }
    if (Registry1745_CheckSessionReady(p) < 0) {
        Registry1745_BeginShutdown(p, ScriptPanel_OpenTextWindow);
        return -1;
    }
    if (Engine_PauseBits(0x800)) {
        s32 best = 0, bestIndex = -1, j;
        for (j = 0; j <= 1; j++) {
            if (((u8 **)gParticipantActors)[j]) {
                ((u8 **)gParticipantActors)[j][0x8f9] = 0;
                if (best < *(s32 *)(((u8 **)gParticipantActors)[j] + 0x28)) {
                    best = *(s32 *)(((u8 **)gParticipantActors)[j] + 0x28);
                    bestIndex = j;
                }
            }
        }
        if (bestIndex >= 0)
            ((u8 **)gParticipantActors)[bestIndex][0x8f9] = 1;
    }
    c->frames++;
    keys = *(vu16 *)0x04000130;
    keys &= 0xfff3;
    flags = 0;
    for (i = 0; i < p->participantCount; i++) {
        u16 value = Slot(p, i);
        if (ModeInput(value) || ModeLeave(value))
            LinkSelection_RecordSlotState(p, value, i, &flags, 0);
        else if (!ModeCancel(value)) {
            if (value == 0xffff) {
                Registry1745_BeginShutdown(p, ScriptPanel_OpenTextWindow);
                return -1;
            }
        } else {
            LinkSelection_RecordSlotState(p, value, i, &flags, 0);
            if (i == p->localPlayer)
                Registry1745_SetState(p, 18, LinkSelection_WaitForInputCompletion);
        }
    }
    LinkSelection_AdvanceBitExchange(p, flags, &keys);
    {
        s32 state = SessionState();
        if (state == 18)
            p->requestedMode = PackKeys(keys, 0x3400);
        else if (p->menuEvent == 4)
            p->requestedMode = PackKeys(keys, 0x2800);
        else {
            p->requestedMode = PackKeys(keys, 0xc00);
            c->elapsed++;
        }
    }
    return 0;
}
