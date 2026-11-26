#include "effects/particles/proximity_emitter.h"
struct ProximityEmitter *TetherRing_CreateFromScript(u32 a, u32 b)
{
    struct ProximityEmitter *emitter = Task_Create(9, 0x268);
    if (emitter) {
        Task_SetCallbacks(emitter, (void (*)(void *))TetherRing_Tick,
                          (void (*)(void *))TetherRing_Destroy);
        if (TetherRing_InitializeFromScript(emitter, a, b) < 0) {
            Task_Kill(emitter);
            return 0;
        }
    }
    return emitter;
}
