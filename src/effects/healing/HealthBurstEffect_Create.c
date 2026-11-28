#include "effects/healing/healing_effect.h"
#include "engine/engine.h"

/* The initializer takes the mode as a u16; this caller passes the word through. */
s32 sub_08235C30(struct HealthBurstEffect *, struct Player *, u32);

struct HealthBurstEffect *HealthBurstEffect_Create(struct Player *owner, u32 mode)
{
    struct HealthBurstEffect *effect = Task_Create(11, sizeof(*effect));

    if (effect != NULL) {
        Task_SetCallbacks(effect, (void (*)(void *))HealthBurstEffect_OnUpdate,
                          (void (*)(void *))SceneEffectGroup_Remove);
        if (sub_08235C30(effect, owner, mode) < 0) {
            Task_Kill(effect);
            return NULL;
        }
    }
    return effect;
}
