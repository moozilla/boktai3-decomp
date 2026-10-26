// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "area_effect_link.h"
#include "game_state.h"
#include "engine.h"
#include "ram.h"
extern u32 gUnk_030053F0;
extern u8 *gUnk_020004F0[];
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
static inline u8 ModeInput(u32 input, u32 mask)
{
    u32 value = input;
    value &= mask;
    if (value == 0xc00)
        return 1;
    return 0;
}
static inline u8 ModeLeave(u32 input, u32 mask)
{
    u32 value = input;
    value &= mask;
    if (value == 0x3400)
        return 1;
    return 0;
}
static inline u8 ModeCancel(u32 input, u32 mask)
{
    u32 value = input;
    asm("" : "+r"(value));
    value &= mask;
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
static inline s32 SessionState(void)
{
    if (!gUnk_020004B4)
        return -1;
    return gUnk_020004B4->unknown1C;
}
s32 LinkSelection_ExchangeParticipantInput(struct AreaEffectLinkState *p)
{
    struct ExchangeCounters *c = (void *)p->sessionData;
    u16 keys;
    u32 flags;
    s32 i;
    u32 mask;
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
    mask = 0x800;
    if ((gEngineFlags | gUnk_030053F0) & mask) {
        s32 best = 0, bestIndex = -1, j;
        for (j = 0; j <= 1; j++) {
            if (gUnk_020004F0[j]) {
                gUnk_020004F0[j][0x8f9] = 0;
                if (best < *(s32 *)(gUnk_020004F0[j] + 0x28)) {
                    best = *(s32 *)(gUnk_020004F0[j] + 0x28);
                    bestIndex = j;
                }
            }
        }
        if (bestIndex >= 0)
            gUnk_020004F0[bestIndex][0x8f9] = 1;
    }
    c->frames++;
    keys = *(vu16 *)0x04000130;
    keys &= 0xfff3;
    flags = 0;
    i = 0;
    if (i < p->participantCount) {
        register u32 initialMask asm("r1") = 0x3c00;
        u32 mask;
        asm("" : "+r"(initialMask));
        mask = initialMask;
        do {
            u16 value = Slot(p, i);
            if (ModeInput(value, mask) || ModeLeave(value, mask))
                LinkSelection_RecordSlotState(p, value, i, &flags, 0);
            else if (!ModeCancel(value, mask)) {
                if (value == 0xffff) {
                    Registry1745_BeginShutdown(p, ScriptPanel_OpenTextWindow);
                    return -1;
                }
            } else {
                LinkSelection_RecordSlotState(p, value, i, &flags, 0);
                if (i == p->localPlayer)
                    Registry1745_SetState(p, 18, LinkSelection_WaitForInputCompletion);
            }
        } while (++i < p->participantCount);
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
