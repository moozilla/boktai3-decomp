#include "global.h"
#include "input.h"
#include "area_effect_link.h"

// MATCHING_CONCESSION: register-pinning
struct LinkReadySessionFlags {
    u8 pad0[0x2c];
    u8 startRequested;
};

struct LinkReadyState {
    u8 pad0[0x18];
    s16 localPlayer;
    u16 count;
    u8 pad1c;
    u8 busy;
    u8 pad1e[5];
    u8 event;
    u8 pad24[4];
    u8 requested;
    u8 pad29[4];
    u8 countdown;
    u16 timer;
    u8 pad30[0xe];
    u16 mode;
    u16 *selection;
    u8 pad44[4];
    s32 status;
    u8 pad4c[0x18];
    u8 presence[2];
    u8 pad66[0xa];
    u8 ready[2];
    u8 pad72[2];
    u8 missed[2];
};
s32 Serial_ReadLatestPacket(u16 *);
u8 Registry1745_ConsumeStateEntry(void *);
u8 Registry1745_HasParticipantCount(void *, s32);
void Registry1745_SetState(void *, u32, void (*)(void));
void Registry1745_ClearParticipantCounters(void *);
void Serial_StopCommunication(void);
void SerialLink_InitMultiplayer(void);
void Sound_PlaySong(u32);
void ScriptPanel_PollSelection(void);
void sub_0810A304(void);
void ScriptPanel_ConfirmSelection(void);
static inline u16 LinkReady_GetSelection(struct LinkReadyState *p, s32 i)
{
    if (i < 0)
        return -1;
    return p->selection[i];
}
static inline u8 LinkReady_IsStartRequested(u16 value, u16 mask)
{
    value &= mask;
    if (value == 0x1400)
        return 1;
    return 0;
}

static inline u8 LinkReady_HasMode(u32 value, u32 mask, u32 expected)
{
    value &= mask;
    if (value == expected)
        return 1;
    return 0;
}

static inline void LinkReady_ClearReady(struct LinkReadyState *p)
{
    s32 i = 1;
    u8 *base;
    do {
        base = p->ready;
        base[i] = 0;
        --i;
    } while ((s32)(base + i) >= (s32)base);
}
static inline u16 LinkReady_AdvanceTimer(u16 *timer)
{
    ++*timer;
    /* Temporary: preserves the ROM's GCSE expression-table and spill order. */
    asm("" : : "m"(*timer));
    return *timer;
}
s32 LinkMenu_WaitForReadyPeers(struct LinkReadyState *p)
{
    u16 packets[4];
    s32 i;
    u8 *event;
    Serial_ReadLatestPacket(packets);
    if (Registry1745_ConsumeStateEntry(p)) {
        p->timer = 0;
        p->requested = 0;
        {
            register struct LinkReadyState *initState asm("r4") = p;
            initState->countdown = 40;
        }
        if (gLinkSession)
            ((struct LinkReadySessionFlags *)gLinkSession)->startRequested = 0;
        LinkReady_ClearReady(p);
    }
    if (!Registry1745_HasParticipantCount(p, 2)) {
        p->requested = 0;
        if (gLinkSession)
            ((struct LinkReadySessionFlags *)gLinkSession)->startRequested = 0;
        for (i = 0; i <= 1; i++) {
            p->presence[i] = 0;
            p->ready[i] = 0;
        }

        if ((++p->timer) > 119) {
            Registry1745_SetState(p, 2, ScriptPanel_PollSelection);
            Serial_StopCommunication();
        }
        return 0;
    }
    if (p->status < 0) {
        if (LinkReady_AdvanceTimer(&p->timer) > 59) {
            Registry1745_SetState(p, 2, ScriptPanel_PollSelection);
            Serial_StopCommunication();
            return 0;
        }
        if (p->status != -3) {
            LinkReady_ClearReady(p);
            for (i = 0; i <= 1; i++) {
                if (packets[i] == 0xffff) {
                    if (p->missed[i] < 255)
                        p->missed[i]++;
                    if (p->missed[i] > 119)
                        p->presence[i] = 0;
                } else {
                    p->missed[i] = 0;
                    p->presence[i] = 1;
                }
            }
        } else if (!p->busy && (gPadState[1] & 8))
            p->requested = 1;
    } else {
        s32 readyCount;
        s32 allReady;
        s32 other;
        for (i = 0; i <= 1; i++) {
            if (packets[i] == 0xffff) {
                if (p->missed[i] < 255)
                    p->missed[i]++;
                if (p->missed[i] > 119)
                    p->presence[i] = 0;
            } else {
                p->missed[i] = 0;
                p->presence[i] = 1;
            }
        }

        p->timer = 0;
        if (packets[p->status] == 0xffff) {
            Registry1745_SetState(p, 2, ScriptPanel_PollSelection);
            Serial_StopCommunication();
            return 0;
        }
        p->localPlayer = p->status;
        p->count = 0;
        readyCount = 0;
        allReady = 1;
        other = 0;
        for (i = 0; i <= 1; i++) {
            u16 value = LinkReady_GetSelection(p, i);
            if ((value != 0xffff) &&
                ((LinkReady_HasMode(value, 0x3c00, 0x400) && ((value & 255) == 0x9c)) ||
                 LinkReady_HasMode(value, 0x3c00, 0x1400))) {
                readyCount++;
                if (p->ready[i] < 255)
                    p->ready[i]++;
                if (p->ready[i] > 89) {
                    p->presence[i] = 2;
                    if (allReady)
                        p->count++;
                } else {
                    allReady = 0;
                    p->presence[i] = 1;
                }
            } else {
                if (value == 0xffff)
                    p->presence[i] = 0;
                else {
                    other = 1;
                    p->presence[i] = 1;
                }
                allReady = 0;
                p->ready[i] = 0;
                if (gLinkSession)
                    ((struct LinkReadySessionFlags *)gLinkSession)->startRequested = 0;
            }
        }

        if (other)
            for (i = 0; i <= 1; i++) {
                if (packets[i] != 0xffff)
                    p->presence[i] = 1;
                p->ready[i] = 0;
            }

        if (LinkReady_IsStartRequested(*p->selection, 0x3c00)) {
            if (gLinkSession)
                ((struct LinkReadySessionFlags *)gLinkSession)->startRequested = 1;
            if (p->countdown) {
                p->countdown--;
                if (p->count != 2 || p->count != readyCount || other)
                    goto waiting;
                if (!p->busy)
                    p->presence[0] = 3;
            } else if (p->count == 2 && p->count == readyCount && !other) {
                Registry1745_SetState(p, 5, sub_0810A304);
                Registry1745_ClearParticipantCounters(p);
                Sound_PlaySong(0xdd);
                return 0;
            } else
                goto waiting;
        } else {
            p->countdown = 40;
            if (!p->busy && p->count == 2 && p->count == readyCount && !other) {
                if ((gPadState[1] & 8) || p->requested) {
                    u16 requestedMode = 0x1400;
                    p->mode = requestedMode;
                    p->requested = 0;
                    if (gLinkSession)
                        ((struct LinkReadySessionFlags *)gLinkSession)->startRequested = 1;
                }
                p->presence[0] = 3;
            } else {
            waiting:
                p->mode = 0x49c;
            }
        }
    }
    event = &p->event;
    if ((*event) == 2) {
        SerialLink_InitMultiplayer();
        Registry1745_SetState(p, 1, ScriptPanel_ConfirmSelection);
    }
    return 0;
}
