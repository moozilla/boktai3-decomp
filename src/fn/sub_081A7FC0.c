#include "global.h"

struct VanargandPair {
    u8 pad0[0xAA];
    u8 state;
    u8 padAB[4];
    u8 active, pending;
    u8 padB1[0xC4 - 0xB1];
    u32 timer;
    u8 padC8[0x56C - 0xC8];
    void *pair;
};

void Vanargand_StopActionSequence(void *);
s32 ScenePair_CountPositiveCounters(void *);
void ScenePanel_UpdatePairConditions(void *);
void ScenePanel_ApplyPairTransition(void *);

static inline u8 ConsumeEntry(struct VanargandPair *p)
{
    u8 *entry = &p->pending;
    if (*entry != 0) {
        *entry = 0;
        *(entry - 1) = 0;
        return TRUE;
    }
    return FALSE;
}

static inline void RequestState(struct VanargandPair *p, u32 state)
{
    Vanargand_StopActionSequence((u8 *)p + 0x21C);
    p->state = state;
}

#define Vanargand_UpdatePairEntry sub_081A7FC0
void Vanargand_UpdatePairEntry(struct VanargandPair *p)
{
    struct VanargandPair *q = p;
    if (ConsumeEntry(p))
        RequestState(p, 16);
    if (p->active != 0) {
        if (ScenePair_CountPositiveCounters(q->pair) <= 1) {
            RequestState(p, 2);
        } else {
            ScenePanel_UpdatePairConditions(q);
            ScenePanel_ApplyPairTransition(q);
        }
    }
    p->timer++;
}
