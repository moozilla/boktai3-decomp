#include "effects/scene/repeated_effect.h"
#include "engine/word_flags.h"
void RepeatedEffect_HandleContact(struct RepeatedEffectCollider *contact,
                                  struct RepeatedEffectCollider *other,
                                  struct RepeatedEffect *effect)
{
    struct RepeatedEffect *initial = effect;
    struct RepeatedEffect *saved = initial;
    u16 *cooldown = &other->cooldown;
    if (!*cooldown) {
        if (!(Flags_MaskedWord(4, &contact->responseFlags)))
            return;
        if (Flags_MaskedWord(4, &contact->contactFlags)) {
            u8 state = initial->state;
            if (state == 0)
                RepeatedEffect_SetState(initial, 2, RepeatedEffect_TickUpperTransition);
            else if (state == 1)
                RepeatedEffect_SetState(initial, 0, RepeatedEffect_TickLowerRecovery);
            other->cooldown = 30;
        } else if (contact->contactFlags & 8) {
            if (initial->state == 0)
                RepeatedEffect_SetState(initial, 1, RepeatedEffect_TickLowerTransition);
            *cooldown = 30;
        } else if (saved->state == 0) {
            u8 *request = &saved->request;
            *request = 1;
        }
    }
}
