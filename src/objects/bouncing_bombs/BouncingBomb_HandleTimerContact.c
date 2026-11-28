#include "objects/bouncing_bombs/bouncing_object.h"
#include "engine/word_flags.h"
void BouncingBomb_HandleTimerContact(struct BouncingContact *contact, void *unused,
                                     struct BouncingObject *object)
{
    struct BouncingObject *self = object;

    if (WordFlags_Test(&contact->flags, 4)) {
        object->timer = 0;
    } else if (contact->flags & 8) {
        if (self->timer > 89)
            self->timer = self->initialTimer;
    }
}
