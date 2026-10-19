#define Vanargand_UpdateTargetFlagTransition sub_081A85AC
#include "global.h"
struct VanargandTarget { u8 pad0[0x20]; u32 flags; };
struct VanargandTransition {
    u8 pad0[0xAA];
    u8 state, previousState;
    u8 padAC[2];
    u8 mode, active, pending;
    u8 padB1[0xC4-0xB1];
    u32 counter;
    u8 padC8[0x10C-0xC8];
    struct VanargandTarget *target;
};
void Vanargand_StopActionSequence(void *);
static inline u8 Vanargand_ConsumePending(struct VanargandTransition *p)
{
    if (p->pending) { p->pending=0; p->active=0; return TRUE; }
    return FALSE;
}
static inline u8 Vanargand_IsZero(u32 value)
{
    if (!value) return TRUE;
    return FALSE;
}
void sub_081A85AC(struct VanargandTransition *p)
{
    if (Vanargand_ConsumePending(p) && p->previousState != 17) {
        u32 state = 17;
        Vanargand_StopActionSequence((u8 *)p+0x21C);
        p->state = state;
    }
    if (p->active) {
        struct VanargandTarget *target = p->target;
        u32 mask = 0x1000;
        u32 flags = target->flags & mask;
        if (!flags) {
            u32 mode = 13;
            p->mode = mode;
            p->counter = flags;
            p->pending = 1;
            return;
        } else {
            u32 mode = 17;
            p->mode = mode;
            p->counter = 0;
            p->pending = 1;
        }
    }
}
