#define LinkMenu_ConsumeStateEntry Registry1745_ConsumeStateEntry
#define LinkMenu_ExchangePayload       sub_0810A8D8
#define LinkMenu_SetState Registry1745_SetState
#define LinkMenu_CheckSession Registry1745_CheckSessionReady
#define LinkMenu_ResetSelection Registry1745_BeginShutdown
#define LinkMenu_ClearExchangeCounters Registry1745_ClearParticipantCounters
#define LinkMenu_SendBytes             sub_081382BC
#define LinkMenu_ReceiveBytes          sub_08138300
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
void sub_0811D384(void *);
void sub_08109C80(void);
void sub_0810A9D0(void);
static inline u8 IsPayloadMode(u16 value)
{
    value &= 0x3c00;
    if (value == 0x1800)
        return 1;
    return 0;
}
s32 sub_0810A8D8(struct AreaEffectLinkState *p)
{
    u8 *session = p->sessionData;
    s32 count, i;
    u8 *incoming;
    if (Registry1745_ConsumeStateEntry(p)) {
        u8 *outgoing;
        Registry1745_ClearParticipantCounters(p);
        outgoing = p->outgoingPayload;
        sub_0811D384(outgoing);
        p->outgoingChecksum = Crc_ComputeCcitt16(outgoing, 0, 0x20);
    }
    if (Registry1745_CheckSessionReady(p) < 0)
        goto fail;
    if (IsPayloadMode(*p->selection)) {
        Registry1745_SetState(p, 14, sub_0810A9D0);
        return 0;
    }
    sub_081382BC(p, session + 0xc0, 0x24);
    count = 0;
    i = 0;
    if (i < p->participantCount) {
        incoming = session + 0xe4;
        do {
            if (sub_08138300(p, incoming, i, 0x24) == 1) {
                Crc_ComputeCcitt16(incoming, 0, 0x20);
                count++;
            }
            incoming += 0x24;
        } while (++i < p->participantCount);
    }
    if (count == p->participantCount && !p->exchangeBusy)
        p->requestedMode = 0x1800;
    if (p->menuElapsed <= 3600)
        return 0;
fail:
    Registry1745_BeginShutdown(p, sub_08109C80);
    return -1;
}
