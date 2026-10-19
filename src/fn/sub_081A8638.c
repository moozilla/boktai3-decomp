#define Vanargand_UpdateProximityTransition sub_081A8638
#include "global.h"
struct VanargandPosition { s16 x, y, z; };
struct VanargandTransition {
    u8 pad0[0x54];
    struct VanargandPosition position;
    u8 pad5C[0xAA-0x5C];
    u8 state;
    u8 padAB[3];
    u8 mode, active, pending;
    u8 padB1[0xC4-0xB1];
    u32 counter;
    u8 padC8[0x110-0xC8];
    struct VanargandPosition *destination;
    u8 pad114[0xF24-0x114];
    u32 alternate;
};
void Vanargand_StopActionSequence(void *);
static inline u8 Vanargand_ConsumePending(struct VanargandTransition *p)
{
    if (p->pending) { p->pending=0; p->active=0; return TRUE; }
    return FALSE;
}
void sub_081A8638(void *object)
{
    struct VanargandTransition *p = object;
    s32 dx, dz;
    struct VanargandPosition *destination, *position;
    if (Vanargand_ConsumePending(p)) {
        u32 alternate = ((struct VanargandTransition *)object)->alternate;
        if (!alternate) {
            u32 mode = 4;
            p->mode = mode;
            p->counter = alternate;
            p->pending = 1;
            return;
        } else {
            u32 state = 19;
            Vanargand_StopActionSequence((u8 *)p+0x21C);
            p->state = state;
        }
    }
    destination = p->destination;
    position = &p->position;
    dx = destination->x - position->x;
    dz = destination->z - position->z;
    if (dx*dx + dz*dz <= 65535) {
        u32 mode = 16;
        p->mode = mode;
        p->counter = 0;
        p->pending = 1;
        {
            u32 state = 17;
            p->state = state;
        }
    } else if (p->active) {
        u32 mode = 13;
        p->mode = mode;
        p->counter = 0;
        p->pending = 1;
    }
    p->counter++;
}
