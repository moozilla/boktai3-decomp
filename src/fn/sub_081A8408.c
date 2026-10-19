#include "global.h"
/* Tentative: action state transition, with restart on flag 0x20000. */
struct VanargandTransition {
    u8 pad0[0x9C];
    u32 flags;
    u8 padA0[0xAA - 0xA0];
    u8 state;
    u8 padAB[3];
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
static inline u32 Vanargand_TestFlags(u32 *flags, u32 mask)
{
    return *flags & mask;
}
void Vanargand_UpdateMode9Transition(void *object)
{
    struct VanargandTransition *p = object;
    if (Vanargand_ConsumePending(p)) {
        if (!((struct VanargandTransition *)object)->alternate) {
            u32 state = 5;
            Vanargand_StopActionSequence((u8 *)p + 0x21C);
            p->state = state;
            return;
        } else {
            u32 state = 9;
            Vanargand_StopActionSequence((u8 *)p + 0x21C);
            p->state = state;
        }
    } else if (p->active) {
        u32 mask = 0x20000;
        if (p->flags & mask) {
            u32 mode = 16;
            p->mode = mode;
            p->counter = 0;
            p->pending = 1;
        } else {
            ScenePanel_UpdatePairConditions(object);
            ScenePanel_ApplyPairTransition(object);
        }
    }
    p->counter++;
}
