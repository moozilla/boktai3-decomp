#include "global.h"
#include "engine/engine.h"
#include "objects/deck_cannons/script_value_rotation_layout.h"

s32 PaletteCycleBackground_Init(struct ScriptValueRotation *);
s32 ScriptValues_TickRotation(struct ScriptValueRotation *);
u32 BackgroundAnimation_Destroy(struct ScriptValueRotation *);

struct ScriptValueRotation *PaletteCycleBackground_Create(void)
{
    struct ScriptValueRotation *rotation = Task_Create(8, sizeof(*rotation));

    if (rotation) {
        Task_SetCallbacks(rotation, (void (*)(void *))ScriptValues_TickRotation,
                          (void (*)(void *))BackgroundAnimation_Destroy);
        if (PaletteCycleBackground_Init(rotation) < 0) {
            Task_Kill(rotation);
            return 0;
        }
    }
    return rotation;
}
