#include "bouncing_object.h"
static inline u32 ContactFlags(const u32 *flags, u32 mask)
{
    return *flags & mask;
}
void BouncingObject_HandleTimerContact(struct BouncingContact *contact, void *unused,
                                       struct BouncingObject *object)
{
    struct BouncingObject *self = object;
    u16 *timer;
    u16 value;
    if (ContactFlags(&contact->flags, 4)) {
        timer = &object->timer;
        value = 0;
    } else if (contact->flags & 8) {
        timer = &self->timer;
        if (*timer <= 89)
            return;
        value = self->initialTimer;
    } else
        return;
    *timer = value;
}

