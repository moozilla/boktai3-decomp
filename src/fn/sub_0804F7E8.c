// MATCHING_CONCESSION: register-pinning
#include "repeated_effect.h"
#define RepeatedEffect_HandleContact sub_0804F7E8
void RepeatedEffect_HandleContact(struct RepeatedEffectCollider *contact,
                                  struct RepeatedEffectCollider *other,
                                  struct RepeatedEffect *effect)
{
    register struct RepeatedEffect *initial asm("r3") = effect;
    struct RepeatedEffect *saved = initial;
    u16 *cooldown = &other->cooldown;
    if (!*cooldown) {
        u32 mask = 4;
        u32 flags;
        if (!(contact->responseFlags & mask))
            return;
        flags = contact->contactFlags;
        if (flags & mask) {
            u8 state = initial->state;
            if (state == 0)
                sub_0804F7D0(initial, 2, sub_0804F9C4);
            else if (state == 1)
                sub_0804F7D0(initial, 0, sub_0804F91C);
            other->cooldown = 30;
        } else if (flags & 8) {
            if (initial->state == 0)
                sub_0804F7D0(initial, 1, sub_0804F8CC);
            *cooldown = 30;
        } else if (saved->state == 0) {
            u8 *request = &saved->request;
            *request = 1;
        }
    }
}
