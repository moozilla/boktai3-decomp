#include "proximity_emitter.h"
struct ProximityEmitter *ProximityEmitter_CreateFromScript(u32 a, u32 b)
{
    struct ProximityEmitter *emitter = Task_Create(9, 0x268);
    if (emitter) {
        Task_SetCallbacks(emitter, (void (*)(void *))ProximityEmitter_Tick,
                          (void (*)(void *))ProximityEmitter_Destroy);
        if (ProximityEmitter_InitializeFromScript(emitter, a, b) < 0) {
            Task_Kill(emitter);
            return 0;
        }
    }
    return emitter;
}
