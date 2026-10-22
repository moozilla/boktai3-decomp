#include "global.h"
struct AxeContactState {
    u8 pad0[0x134];
    struct {
        u16 v[4];
        u8 pad[0x54 - 8];
    } body;
    struct {
        u16 v[4];
    } attack;
    u8 pad190[0xcab - 0x190];
    u8 flags;
};
// confidence=low: contact sets a latch and disables both actor colliders.
#define Axe_LatchColliderContact sub_080F7580
void Axe_LatchColliderContact(void *a, void *b, struct AxeContactState *s)
{
    if (!(s->flags & 1)) {
        s->flags |= 1;
        {
            typeof(s->attack) *p = &s->attack;
            p->v[3] |= 4;
        }
        {
            typeof(s->body) *p = &s->body;
            p->v[3] |= 4;
        }
    }
}
