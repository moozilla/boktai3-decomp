#include "scene_actor_reset.h"
struct VornDeferredScript {
    u8 unknown0[0x624];
    u16 script;
    u8 duration, elapsed;
};

// confidence=low: context script 624 is invoked once the byte-sized delay expires.
#define Vorn_UpdateDelayedScript sub_080B2354
void Vorn_UpdateDelayedScript(struct SceneActorRegistryResetView *actor)
{
    struct VornDeferredScript *context = actor->context;
    if (context->script) {
        context->elapsed++;
        if (context->elapsed >= context->duration) {
            Script_Call(context->script, 0);
            context->script = 0;
            context->duration = 0;
            context->elapsed = 0;
        }
    }
}
