// MATCHING_CONCESSION: register-pinning
#include "repeated_effect.h"
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
                RepeatedEffect_SetState(initial, 2, RepeatedEffect_TickUpperTransition);
            else if (state == 1)
                RepeatedEffect_SetState(initial, 0, RepeatedEffect_TickLowerRecovery);
            other->cooldown = 30;
        } else if (flags & 8) {
            if (initial->state == 0)
                RepeatedEffect_SetState(initial, 1, RepeatedEffect_TickLowerTransition);
            *cooldown = 30;
        } else if (saved->state == 0) {
            u8 *request = &saved->request;
            *request = 1;
        }
    }
}
