#include "global.h"

// Tentative Vanargand ownership; field roles follow the mode transition accesses.

struct VanargandModeActor {
    u8 unknown0[0xAA];
    u8 requestedMode;
    u8 currentMode;
    u8 unknownAC[3];
    u8 active;
    u8 entering;
    u8 unknownB1[0xC4 - 0xB1];
    u32 timer;
    u8 unknownC8[0x5C2 - 0xC8];
    s16 variant;
};

void Vanargand_StopActionSequence(void *);
void ScenePanel_UpdatePairConditions(void *);
void ScenePanel_ApplyPairTransition(void *);

static inline u8 ConsumeEntry(struct VanargandModeActor *p)
{
    u8 *entry = &p->entering;
    if (*entry != 0) {
        *entry = 0;
        *(entry - 1) = 0;
        return TRUE;
    }
    return FALSE;
}

static inline void RequestMode(struct VanargandModeActor *p, u32 mode)
{
    Vanargand_StopActionSequence((u8 *)p + 0x21C);
    p->requestedMode = mode;
}

static inline void ResetTimer(struct VanargandModeActor *p)
{
    p->timer = 0;
}

void Vanargand_UpdateModeCycle(struct VanargandModeActor *p)
{
    struct VanargandModeActor *q = p;
    if (ConsumeEntry(p)) {
        if (p->variant == 1)
            RequestMode(q, 16);
        else
            RequestMode(q, 7);
        ResetTimer(q);
    }
    if (q->active != 0) {
        if (q->timer == 0 && q->currentMode == 16) {
            RequestMode(q, 7);
        } else if (q->currentMode == 7) {
            RequestMode(q, 8);
        } else if (q->currentMode == 8) {
            RequestMode(q, 16);
        } else {
            ScenePanel_UpdatePairConditions(p);
            ScenePanel_ApplyPairTransition(p);
        }
        q->timer++;
    }
}
