#include "effects/healing/healing_effect.h"
#include "engine/engine.h"

struct HealthRestoreEffect *HealthRestoreEffect_Create(struct Player *owner)
{
    struct HealthRestoreEffect *effect = Task_Create(11, sizeof(*effect));

    if (effect != NULL) {
        Task_SetCallbacks(effect, (void (*)(void *))HealthRestoreEffect_OnUpdate,
                          (void (*)(void *))HealthRestoreEffect_OnDestroy);
        if (HealthRestoreEffect_Init(effect, owner) < 0) {
            Task_Kill(effect);
            return NULL;
        }
    }
    return effect;
}
