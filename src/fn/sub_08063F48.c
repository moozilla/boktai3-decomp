#include "roaming_object.h"
#define RoamingObjectB_SetState sub_08063F48
struct RoamingStateTable {
    void (*entries[3])(struct RoamingObject *);
};
extern const struct RoamingStateTable gUnk_0824DF98;
void RoamingObjectB_SetState(struct RoamingObject *owner, u16 state)
{
    struct RoamingStateTable table = gUnk_0824DF98;
    owner->state = state;
    owner->flashCount = 0;
    owner->update = table.entries[owner->state];
}
