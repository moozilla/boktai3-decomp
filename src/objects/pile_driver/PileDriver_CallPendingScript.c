#include "objects/pile_driver/pile_driver_effects.h"
struct PileDriverScriptArguments {
    u32 count;
    s32 *values;
};
void PileDriver_CallPendingScript(struct PileDriver *input, u32 value)
{
    struct PileDriverEffectsView *panel = (void *)input;
    u32 scriptId = panel->pendingScriptId;
    if (scriptId) {
        s32 argument;
        struct PileDriverScriptArguments arguments;
        arguments.count = (arguments.count & 0xFFFF0000) | 1;
        argument = value;
        arguments.values = &argument;
        Script_Call(scriptId, &arguments);
    }
}
