// MATCHING_CONCESSION: register-pinning
#include "global.h"
// The original copies p at entry (adds r5, r4) and uses the copy only in the
// retry branch; r5 is reused for 0xFFFF in the init loop. A plain `q = p` copy
// gets r7 and swaps roles, so q is pinned to r5.

struct PollPanel {
    u8 pad0[0x1d];
    u8 role;
    u8 pad1e[0x10];
    u16 retryTimer;
    u32 counter;
    u8 pad34[10];
    u16 f3e;
    u8 pad40[0x24];
    u8 buttons[2];
    u8 pad66[2];
    u16 masks[2];
};

u8 Registry1745_ConsumeStateEntry(struct PollPanel *);
void Registry1745_SetState(struct PollPanel *, u32, void *);
void SerialLink_InitMultiplayer(void);
s32 Serial_DetectRoleAndEnableInterrupt(void);
void Serial_StopCommunication(void);
void ScriptPanel_WaitForSelectionData(void);

s32 ScriptPanel_PollSelection(struct PollPanel *p);
static inline void ScriptPanel_Retry(struct PollPanel *q)
{
    q->retryTimer++;
    if (q->retryTimer > 29) {
        u8 *base, *button;
        u32 zero;
        Registry1745_SetState(q, 2, ScriptPanel_PollSelection);
        Serial_StopCommunication();
        base = q->buttons;
        zero = 0;
        button = &q->buttons[1];
        do {
            *button-- = zero;
        } while ((s32)button >= (s32)base);
    }
}
// confidence=low: starts multiplayer serial, then waits for a role and retries on timeout.
s32 ScriptPanel_PollSelection(struct PollPanel *p)
{
    struct PollPanel *q = p;
    s32 i;
    s32 role;
    if (Registry1745_ConsumeStateEntry(p)) {
        DEBUG_NOP();
        SerialLink_InitMultiplayer();
        for (i = 0; i <= 1; i++) {
            p->buttons[i] = 0;
            p->masks[i] |= 0xffff;
        }
        p->f3e = 0;
        p->retryTimer = 0;
        return 0;
    }
    if (p->counter > 0x13) {
        role = Serial_DetectRoleAndEnableInterrupt();
        if (role >= 0) {
            p->role = role;
            Registry1745_SetState(p, 3, ScriptPanel_WaitForSelectionData);
        } else {
            ScriptPanel_Retry(q);
        }
    }
    return 0;
}
