#define LinkMenu_ConsumeStateEntry     Registry1745_ConsumeStateEntry
#define LinkMenu_SetState              Registry1745_SetState
#define LinkMenu_CheckSession          Registry1745_CheckSessionReady
#define LinkMenu_ResetSelection        Registry1745_BeginShutdown
#define LinkMenu_ClearExchangeCounters Registry1745_ClearParticipantCounters
#define gLinkMenuState                 gUnk_020004B4
#include "global.h"
#include "area_effect_link.h"
#include "engine.h"
u8 Registry1745_ConsumeStateEntry(void *);
void Registry1745_ClearParticipantCounters(void *);
s32 Registry1745_CheckSessionReady(void *);
void Registry1745_BeginShutdown(void *, void (*)(void));
void Registry1745_SetState(void *, u32, void (*)(void));
s32 Crc_ComputeCcitt16(void *, s32, s32);
u8 ByteSequence_AdvanceEncodedValue(void *, void *, s32);
u8 LinkSession_ReceiveSelectionByte(void *, void *, s32, s32);
void SlotState_ResetInactiveMode(void *);
void ScriptPanel_OpenTextWindow(void);
void LinkMenu_ShowChoiceMessage(void);
s32 LinkMenu_ExchangeChoices(struct AreaEffectLinkState *p)
{
    u8 *session = p->sessionData;
    u8 *incoming;
    if (Registry1745_ConsumeStateEntry(p)) {
        Registry1745_ClearParticipantCounters(p);
        if (gUnk_020004B4 && gUnk_020004B4->localPlayer == 0)
            p->exchangeText = Crc_ComputeCcitt16(&p->exchangeValue, 0, 2);
    }
    if (Registry1745_CheckSessionReady(p) < 0) {
        Registry1745_BeginShutdown(p, ScriptPanel_OpenTextWindow);
        return -1;
    }
    if (gUnk_020004B4 && gUnk_020004B4->localPlayer == 0)
        ByteSequence_AdvanceEncodedValue(p, session + 0x12c, 4);
    incoming = session + 0x130;
    if (LinkSession_ReceiveSelectionByte(p, incoming, 0, 4) == 1) {
        Crc_ComputeCcitt16(incoming, 0, 2);
        SlotState_ResetInactiveMode(session);
        gRandomIndex = *(u16 *)(session + 0x60) & 0x3ff;
        Registry1745_SetState(p, 10, LinkMenu_ShowChoiceMessage);
    }
    return 0;
}
