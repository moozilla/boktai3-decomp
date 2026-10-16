#include "scene_actor_control_motion.h"
extern const u8 gUnk_08605C28[];
void SceneActorControl_SetDefaultPreset(struct SceneActorRegistryResetView *actor)
{
    ((struct SceneActorControlPresetView *)actor)->preset284 = gUnk_08605C28;
}
