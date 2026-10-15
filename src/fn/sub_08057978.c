#include "scene_panel_effects.h"
struct ScenePanelScriptArguments {
    u32 count;
    s32 *values;
};
void ScenePanel_CallPendingScript(struct ScenePanel *input, u32 value)
{
    struct ScenePanelEffectsView *panel = (void *)input;
    u32 scriptId = panel->pendingScriptId;
    if (scriptId) {
        s32 argument;
        struct ScenePanelScriptArguments arguments;
        arguments.count = (arguments.count & 0xFFFF0000) | 1;
        argument = value;
        arguments.values = &argument;
        Script_Call(scriptId, &arguments);
    }
}
