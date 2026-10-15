// MATCHING_CONCESSION: register-pinning
#include "bouncing_object.h"
#define BouncingObject_HandleTimerContact sub_08068A3C
static inline u32 ContactFlags(const u32 *flags, u32 mask)
{
    return *flags & mask;
}
void BouncingObject_HandleTimerContact(struct BouncingContact *contact, void *unused, struct BouncingObject *input)
{
    register struct BouncingObject *early asm("r3") = input;
    register struct BouncingObject *object asm("ip") = early;
    {
        u16 *timer;
        u32 value;
        if (ContactFlags(&contact->flags, 4)) {
            timer = &early->timer;
            value = 0;
        } else if (contact->flags & 8) {
            timer = &object->timer;
            if (*timer <= 89) return;
            value = object->initialTimer;
        } else {
            return;
        }
        *timer = value;
    }
}
