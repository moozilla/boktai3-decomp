// MATCHING_CONCESSION: register-pinning
#include "global.h"

/* Selects the next transition after the active timer passes 120. */
struct VanargandTransition {
    u8 pad0[0x9C];
    u32 flags;
    u8 padA0[0xAA - 0xA0];
    u8 state;
    u8 previousState, phase, subphase;
    u8 mode, active, pending;
    u8 padB1[0xC4 - 0xB1];
    u32 counter;
    u8 padC8[0xF24 - 0xC8];
    u32 alternate;
};
void Vanargand_StopActionSequence(void *);
void ScenePanel_UpdatePairConditions(void *);
void ScenePanel_ApplyPairTransition(void *);
static inline u8 Vanargand_ConsumePending(struct VanargandTransition *p)
{
    if (p->pending) {
        p->pending = 0;
        p->active = 0;
        return TRUE;
    }
    return FALSE;
}
void Vanargand_UpdateTimedAlternateTransition(void *object)
{
    struct VanargandTransition *p = object;
    if (Vanargand_ConsumePending(p)) {
        u32 state = 20;
        Vanargand_StopActionSequence((u8 *)p + 0x21C);
        p->state = state;
    }
    if (p->active && p->counter > 120) {
        u32 state;
        u32 alternate = ((struct VanargandTransition *)object)->alternate;
        if (!alternate) {
            u32 mode = 13;
            u8 *field = &p->phase;
            asm("" : "+r"(mode), "+r"(field));
            *field++ = alternate;
            *field++ = alternate;
            *field = mode;
            p->counter = alternate;
            field += 2;
            asm("" : "+r"(field));
            state = 1;
            *field = state;
        } else {
            u32 zero = 0, mode = 18;
            p->phase = zero;
            p->subphase = zero;
            p->mode = mode;
            p->counter = zero;
            p->pending = 1;
            state = 19;
        }
        Vanargand_StopActionSequence((u8 *)p + 0x21C);
        p->state = state;
    } else {
        p->counter++;
    }
}
