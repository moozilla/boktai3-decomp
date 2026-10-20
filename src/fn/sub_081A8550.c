#include "global.h"

struct VanargandEntry {
    u8 pad0[0xAA];
    u8 state;
    u8 padAB[3];
    u8 mode, active, pending;
    u8 padB1[0xC4 - 0xB1];
    u32 counter;
};

void Vanargand_StopActionSequence(void *);

static inline u8 Vanargand_ConsumePending(struct VanargandEntry *p)
{
    if (p->pending) {
        p->pending = 0;
        p->active = 0;
        return TRUE;
    }
    return FALSE;
}

void Vanargand_UpdateEntryTransition(struct VanargandEntry *p)
{
    if (Vanargand_ConsumePending(p)) {
        u32 state = 18;
        Vanargand_StopActionSequence((u8 *)p + 0x21C);
        p->state = state;
    }
    if (p->active) {
        u32 mode = 13;
        p->mode = mode;
        p->counter = 0;
        p->pending = 1;
    }
}
