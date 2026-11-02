#include "global.h"

/* Selects the next transition after the active timer passes 120. */
enum {
    TRANSITION_PHASE,
    TRANSITION_SUBPHASE,
    TRANSITION_MODE,
    TRANSITION_ACTIVE,
    TRANSITION_PENDING,
    TRANSITION_FIELD_COUNT
};

struct VanargandTransition {
    u8 pad0[0x9C];
    u32 flags;
    u8 padA0[0xAA - 0xA0];
    u8 state;
    u8 previousState;
    u8 transition[TRANSITION_FIELD_COUNT];
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
    if (p->transition[TRANSITION_PENDING]) {
        p->transition[TRANSITION_PENDING] = 0;
        p->transition[TRANSITION_ACTIVE] = 0;
        return TRUE;
    }
    return FALSE;
}
static inline u8 *Vanargand_TransitionFields(struct VanargandTransition *owner)
{
    return owner->transition;
}
void Vanargand_UpdateTimedAlternateTransition(void *object)
{
    struct VanargandTransition *p = object;
    if (Vanargand_ConsumePending(p)) {
        u32 state = 20;
        Vanargand_StopActionSequence((u8 *)p + 0x21C);
        p->state = state;
    }
    if (p->transition[TRANSITION_ACTIVE] && p->counter > 120) {
        u32 state;
        const struct VanargandTransition *source = object;
        u32 alternate = source->alternate;
        if (!alternate) {
            u32 mode = 13;
            u8 *field = Vanargand_TransitionFields(p);
            *field++ = alternate;
            *field++ = alternate;
            *field = mode;
            p->counter = alternate;
            field += 2;
            state = 1;
            *field = state;
        } else {
            u32 zero = 0, mode = 18;
            p->transition[TRANSITION_PHASE] = zero;
            p->transition[TRANSITION_SUBPHASE] = zero;
            p->transition[TRANSITION_MODE] = mode;
            p->counter = zero;
            p->transition[TRANSITION_PENDING] = 1;
            state = 19;
        }
        Vanargand_StopActionSequence((u8 *)p + 0x21C);
        p->state = state;
    } else {
        p->counter++;
    }
}
