#include "proximity_emitter.h"
#define ProximityEmitter_CreateFromScript sub_080520E8
struct ProximityEmitter *ProximityEmitter_CreateFromScript(u32 a, u32 b)
{
    struct ProximityEmitter *emitter = Task_Create(9, 0x268);
    if (emitter) {
        Task_SetCallbacks(emitter, (void (*)(void *))sub_08051DA4, (void (*)(void *))sub_08051DE4);
        if (sub_08051EA8(emitter, a, b) < 0) {
            Task_Kill(emitter);
            return 0;
        }
    }
    return emitter;
}
