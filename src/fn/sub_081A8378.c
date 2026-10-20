#include "global.h"

struct VanargandVariant {
    u8 pad0[0xAA];
    u8 state, currentState;
    u8 padAC[3];
    u8 active, pending;
    u8 padB1[0xC4 - 0xB1];
    u32 timer;
    u8 padC8[0x5C2 - 0xC8];
    s16 variant;
};

void Vanargand_StopActionSequence(void *);
void ScenePanel_UpdatePairConditions(void *);
void ScenePanel_ApplyPairTransition(void *);

static inline u8 ConsumeEntry(struct VanargandVariant *p)
{
    u8 *entry = &p->pending;
    if (*entry != 0) {
        *entry = 0;
        *(entry - 1) = 0;
        return TRUE;
    }
    return FALSE;
}

static inline void RequestState(struct VanargandVariant *p, u32 state)
{
    Vanargand_StopActionSequence((u8 *)p + 0x21C);
    p->state = state;
}

#define Vanargand_UpdateVariantEntry sub_081A8378
void Vanargand_UpdateVariantEntry(struct VanargandVariant *p)
{
    struct VanargandVariant *q = p;
    if (ConsumeEntry(p)) {
        if (q->variant == 0)
            RequestState(p, 16);
        else
            RequestState(p, 13);
    }
    if (p->active != 0) {
        if (p->currentState == 12) {
            ScenePanel_UpdatePairConditions(q);
            ScenePanel_ApplyPairTransition(q);
        } else {
            RequestState(p, 12);
        }
    }
    p->timer++;
}
