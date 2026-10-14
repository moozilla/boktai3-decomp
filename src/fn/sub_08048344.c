#include "contact_burst.h"
#define ContactBurst_Create sub_08048344
#define ContactBurst_Update sub_080480A4
#define ContactBurst_Destroy sub_0804822C
#define ContactBurst_Initialize sub_08048248
struct ContactBurst *ContactBurst_Create(const struct ContactPosition *world)
{
    struct ContactBurst *effect = Task_Create(8, sizeof(*effect));
    if (effect) {
        Task_SetCallbacks(effect, (void (*)(void *))ContactBurst_Update, (void (*)(void *))ContactBurst_Destroy);
        if (ContactBurst_Initialize(effect, world) < 0) {
            Task_Kill(effect);
            return NULL;
        }
    }
    return effect;
}
