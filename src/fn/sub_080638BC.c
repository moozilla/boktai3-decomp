#include "roaming_object.h"
struct RoamingStateTable {
    void (*entries[3])(struct RoamingObject *);
};
extern const struct RoamingStateTable gUnk_0824DF8C;
void RoamingObject_SetState(struct RoamingObject *owner, u16 state)
{
    struct RoamingStateTable table = gUnk_0824DF8C;
    owner->state = state;
    owner->flashCount = 0;
    owner->update = table.entries[owner->state];
}
