#define LinkMenu_ConsumeStateEntry Registry1745_ConsumeStateEntry
#define LinkMenu_ExchangeChoices       sub_0810A5C0
#define LinkMenu_SetState Registry1745_SetState
#define LinkMenu_CheckSession Registry1745_CheckSessionReady
#define LinkMenu_ResetSelection Registry1745_BeginShutdown
#define LinkMenu_ClearExchangeCounters Registry1745_ClearParticipantCounters
#define LinkMenu_SendBytes             sub_081382BC
#define LinkMenu_ReceiveBytes          sub_08138300
#define LinkMenu_ClampChoice           sub_0810BD8C
#define gLinkMenuState                 gUnk_020004B4
#include "global.h"
#include "area_effect_link.h"
u8 Registry1745_ConsumeStateEntry(void *);
void Registry1745_ClearParticipantCounters(void *);
s32 Registry1745_CheckSessionReady(void *);
void Registry1745_BeginShutdown(void *, void (*)(void));
void Registry1745_SetState(void *, u32, void (*)(void));
s32 Crc_ComputeCcitt16(void *, s32, s32);
u8 sub_081382BC(void *, void *, s32);
u8 sub_08138300(void *, void *, s32, s32);
void sub_0810BD8C(void *);
void sub_08109C80(void);
void sub_0810A698(void);
extern u32 gUnk_03005308;
s32 sub_0810A5C0(struct AreaEffectLinkState *p)
{
    u8 *session = p->sessionData;
    u8 *incoming;
    if (Registry1745_ConsumeStateEntry(p)) {
        Registry1745_ClearParticipantCounters(p);
        if (gUnk_020004B4 && gUnk_020004B4->localPlayer == 0)
            p->exchangeText = Crc_ComputeCcitt16(&p->exchangeValue, 0, 2);
    }
    if (Registry1745_CheckSessionReady(p) < 0) {
        Registry1745_BeginShutdown(p, sub_08109C80);
        return -1;
    }
    if (gUnk_020004B4 && gUnk_020004B4->localPlayer == 0)
        sub_081382BC(p, session + 0x12c, 4);
    incoming = session + 0x130;
    if (sub_08138300(p, incoming, 0, 4) == 1) {
        Crc_ComputeCcitt16(incoming, 0, 2);
        sub_0810BD8C(session);
        gUnk_03005308 = *(u16 *)(session + 0x60) & 0x3ff;
        Registry1745_SetState(p, 10, sub_0810A698);
    }
    return 0;
}
