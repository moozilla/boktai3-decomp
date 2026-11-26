#include "objects/bouncing_bombs/bouncing_object.h"
#include "engine/word_flags.h"
void BouncingBomb_HandleTimerContact(struct BouncingContact *contact, void *unused,
                                       struct BouncingObject *object)
{
    struct BouncingObject *self = object;
    u16 *timer;
    u16 value;
    if (WordFlags_Test(&contact->flags, 4)) {
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

