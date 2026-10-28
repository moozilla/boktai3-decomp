#include "global.h"
#include "input.h"
#include "area_effect_link.h"

#ifdef NONMATCHING
// fuzzy: 87.1%
// fix.py best (sweep after the inlined-insn fix)
// fix.py best version (reprinted C)
// Packet handling and readiness are recovered; register allocation and block placement differ.
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
void Registry1745_SetState(void *, u32, void *);
void Registry1745_ClearParticipantCounters(void *);
void Serial_StopCommunication(void);
void SerialLink_InitMultiplayer(void);
void Sound_PlaySong(u32);
void ScriptPanel_PollSelection(void);
void sub_0810A304(void);
void ScriptPanel_ConfirmSelection(void);
static inline u16 Slot(struct LinkReadyState *p, s32 i)
{
    if (i < 0)
        return -1;
    return p->selection[i];
}
static inline u8 Mode(u32 value, u32 mask, u32 expected)
{
    value &= mask;
    if (value == expected)
        return 1;
    return 0;
}
static inline u8 Mode_1(u32 value, u32 mask)
{
    value &= mask;
    if (value == 0x1400)
        return 1;
    return 0;
}

inline static u8 Mode_WaitForReadyPeers(u32 value, u32 mask, u32 expected)
{
    value &= mask;
    if (expected == value)
        return 1;
    return 0;
}

s32 LinkMenu_WaitForReadyPeers(struct LinkReadyState *p)
{
    u16 packets[4];
    s32 i;
    u8 *event;
    Serial_ReadLatestPacket(packets);
    if (Registry1745_ConsumeStateEntry(p)) {
        u8 *base;
        u8 *q;
        p->timer = 0;
        p->requested = 0;
        p->countdown = 40;
        if (((u8 *)gLinkSession))
            ((u8 *)gLinkSession)[0x2c] = 0;
        base = p->ready;
        q = &p->ready[1];
        do {
            *(q--) = 0;
        } while (((s32)q) >= ((s32)base));
    }
    if (!Registry1745_HasParticipantCount(p, 2)) {
        p->requested = 0;
        if (((u8 *)gLinkSession))
            ((u8 *)gLinkSession)[0x2c] = 0;
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
        if ((++p->timer) > 59) {
        disconnect:
            Registry1745_SetState(p, 2, ScriptPanel_PollSelection);

            Serial_StopCommunication();
            return 0;
        }
        if (p->status != (-3)) {
            u8 *base;
            u8 *q;
            base = p->ready;
            q = &p->ready[1];
            do {
                *(q--) = 0;
            } while (((s32)q) >= ((s32)base));
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

        } else if ((!p->busy) && (gPadState[1] & 8))
            p->requested = 1;
    } else {
        s32 readyCount;
        s32 allReady;
        s32 other;
        u32 mask;
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
        if (packets[p->status] == 0xffff)
            goto disconnect;
        p->localPlayer = p->status;
        p->count = 0;
        readyCount = 0;
        allReady = 1;
        other = 0;
        mask = 0x3c00;
        for (i = 0; i <= 1; i++) {
            u16 value = Slot(p, i);
            if ((value != 0xffff) &&
                ((Mode_WaitForReadyPeers(value, mask, 0x400) && ((value & 255) == 0x9c)) ||
                 Mode_WaitForReadyPeers(value, mask, 0x1400))) {
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
                if (((u8 *)gLinkSession))
                    ((u8 *)gLinkSession)[0x2c] = 0;
            }
        }

        if (other)
            for (i = 0; i <= 1; i++) {
                if (packets[i] != 0xffff)
                    p->presence[i] = 1;
                p->ready[i] = 0;
            }

        if (Mode_1(*p->selection, mask)) {
            if (((u8 *)gLinkSession))
                ((u8 *)gLinkSession)[0x2c] = 1;
            if (p->countdown) {
                p->countdown--;
                if (((p->count != 2) || (p->count != readyCount)) || other)
                    goto waiting;
                if (!p->busy)
                    p->presence[0] = 3;
            } else if (((p->count == 2) && (p->count == readyCount)) && (!other)) {
                Registry1745_SetState(p, 5, sub_0810A304);
                Registry1745_ClearParticipantCounters(p);
                Sound_PlaySong(0xdd);
                return 0;
            } else
                goto waiting;
        } else {
            p->countdown = 40;
            if ((((!p->busy) && (p->count == 2)) && (p->count == readyCount)) && (!other)) {
                if ((gPadState[1] & 8) || p->requested) {
                    {
                        u16 k1 = 0x1400;
                        p->mode = k1;
                    }
                    p->requested = 0;
                    if (((u8 *)gLinkSession))
                        ((u8 *)gLinkSession)[0x2c] = 1;
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

#else
INCLUDE_ASM("asm/nonmatching", LinkMenu_WaitForReadyPeers);
#endif
