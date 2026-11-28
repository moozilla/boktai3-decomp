#include "global.h"
#include "link/selection/link_selection_mode.h"
#include "engine/input.h"
#include "scene/areas/area_effect_link.h"

s32 Serial_ReadLatestPacket(u16 *);
u8 LinkSession_ConsumeStateEntry(void *);
u8 LinkSession_HasParticipantCount(void *, s32);
void LinkSession_SetState(void *, u32, void (*)(void));
void LinkSession_ClearParticipantCounters(void *);
void Serial_StopCommunication(void);
void SerialLink_InitMultiplayer(void);
void Sound_PlaySong(u32);
void ScriptPanel_PollSelection(void);
void sub_0810A304(void);
void ScriptPanel_ConfirmSelection(void);
static inline u16 SelectionAt(struct AreaEffectLinkState *p, s32 index)
{
    if (index < 0)
        return -1;
    return p->selection[index];
}
static inline u8 LinkReady_HasMode(u32 value, u32 mask, u32 expected)
{
    if ((value & mask) == expected)
        return 1;
    return 0;
}
s32 LinkMenu_WaitForReadyPeers(struct AreaEffectLinkState *p)
{
    u16 packets[4];
    s32 i;

    Serial_ReadLatestPacket(packets);
    if (LinkSession_ConsumeStateEntry(p)) {
        p->stateTimer = 0;
        p->startQueued = 0;
        /* Storing through a pointer to a copy keeps gcse from sharing this field address
         * with the later countdown accesses. */
        {
            struct AreaEffectLinkState *state = p;
            struct AreaEffectLinkState **session = &state;

            (*session)->startCountdown = 40;
        }
        if (gLinkSession)
            gLinkSession->menuInputLocked = 0;
        for (i = 0; i < 2; i++)
            p->readyFrames[i] = 0;
    }
    if (!LinkSession_HasParticipantCount(p, 2)) {
        p->startQueued = 0;
        if (gLinkSession)
            gLinkSession->menuInputLocked = 0;
        for (i = 0; i < 2; i++) {
            p->peerStates[i] = 0;
            p->readyFrames[i] = 0;
        }
        if (++p->stateTimer > 119) {
            LinkSession_SetState(p, 2, ScriptPanel_PollSelection);
            Serial_StopCommunication();
        }
        return 0;
    }
    if (p->playerSlot < 0) {
        p->stateTimer++;
        if (p->stateTimer > 59) {
            LinkSession_SetState(p, 2, ScriptPanel_PollSelection);
            Serial_StopCommunication();
            return 0;
        }
        if (p->playerSlot != -3) {
            for (i = 0; i < 2; i++)
                p->readyFrames[i] = 0;
            for (i = 0; i < 2; i++) {
                if (packets[i] == 0xffff) {
                    if (p->missedFrames[i] < 255)
                        p->missedFrames[i]++;
                    if (p->missedFrames[i] > 119)
                        p->peerStates[i] = 0;
                } else {
                    p->missedFrames[i] = 0;
                    p->peerStates[i] = 1;
                }
            }
        } else if (!p->exchangeBusy && (gPadState[1] & 8)) {
            p->startQueued = 1;
        }
    } else {
        s32 readyCount;
        s32 allReady;
        s32 other;

        for (i = 0; i < 2; i++) {
            if (packets[i] == 0xffff) {
                if (p->missedFrames[i] < 255)
                    p->missedFrames[i]++;
                if (p->missedFrames[i] > 119)
                    p->peerStates[i] = 0;
            } else {
                p->missedFrames[i] = 0;
                p->peerStates[i] = 1;
            }
        }
        p->stateTimer = 0;
        if (packets[p->playerSlot] == 0xffff) {
            LinkSession_SetState(p, 2, ScriptPanel_PollSelection);
            Serial_StopCommunication();
            return 0;
        }
        p->localPlayer = p->playerSlot;
        p->participantCount = 0;
        readyCount = 0;
        allReady = 1;
        other = 0;
        for (i = 0; i < 2; i++) {
            u16 value = SelectionAt(p, i);
            if (value != 0xffff &&
                ((LinkReady_HasMode(value, 0x3c00, 0x400) && (value & 0xff) == 0x9c) ||
                 LinkReady_HasMode(value, 0x3c00, 0x1400))) {
                readyCount++;
                if (p->readyFrames[i] < 255)
                    p->readyFrames[i]++;
                if (p->readyFrames[i] > 89) {
                    p->peerStates[i] = 2;
                    if (allReady)
                        p->participantCount++;
                } else {
                    allReady = 0;
                    p->peerStates[i] = 1;
                }
            } else {
                if (value == 0xffff) {
                    p->peerStates[i] = 0;
                } else {
                    other = 1;
                    p->peerStates[i] = 1;
                }
                allReady = 0;
                p->readyFrames[i] = 0;
                if (gLinkSession)
                    gLinkSession->menuInputLocked = 0;
            }
        }
        if (other) {
            for (i = 0; i < 2; i++) {
                if (packets[i] != 0xffff)
                    p->peerStates[i] = 1;
                p->readyFrames[i] = 0;
            }
        }
        if (LinkSelection_IsStart(*p->selection, 0x3c00)) {
            if (gLinkSession)
                gLinkSession->menuInputLocked = 1;
            if (p->startCountdown) {
                p->startCountdown--;
                if (p->participantCount == 2 && p->participantCount == readyCount && !other) {
                    if (!p->exchangeBusy)
                        p->peerStates[0] = 3;
                } else {
                    p->requestedMode = 0x49c;
                }
            } else if (p->participantCount == 2 && p->participantCount == readyCount && !other) {
                LinkSession_SetState(p, 5, sub_0810A304);
                LinkSession_ClearParticipantCounters(p);
                Sound_PlaySong(0xdd);
                return 0;
            } else {
                p->requestedMode = 0x49c;
            }
        } else {
            p->startCountdown = 40;
            if (!p->exchangeBusy && p->participantCount == 2 && p->participantCount == readyCount &&
                !other) {
                if ((gPadState[1] & 8) || p->startQueued) {
                    u16 mode = 0x1400;
                    p->requestedMode = mode;
                    p->startQueued = 0;
                    if (gLinkSession)
                        gLinkSession->menuInputLocked = 1;
                }
                p->peerStates[0] = 3;
            } else {
                p->requestedMode = 0x49c;
            }
        }
    }
    if (p->menuEvent == 2) {
        SerialLink_InitMultiplayer();
        LinkSession_SetState(p, 1, ScriptPanel_ConfirmSelection);
    }
    return 0;
}
