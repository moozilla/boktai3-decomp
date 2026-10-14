#include "contact_burst.h"
struct ContactBurst *ContactBurst_Create(const struct ContactPosition *world)
{
    struct ContactBurst *effect = Task_Create(8, sizeof(*effect));
    if (effect) {
        Task_SetCallbacks(effect, (void (*)(void *))ContactBurst_Update,
                          (void (*)(void *))ContactBurst_Destroy);
        if (ContactBurst_Initialize(effect, world) < 0) {
            Task_Kill(effect);
            return NULL;
        }
    }
    return effect;
}
