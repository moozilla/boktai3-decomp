#include "scene_panel_subject.h"
#define ScenePanel_Create sub_0805CB38
struct ScenePanel *ScenePanel_Create(u32 actorId)
{
    struct ScenePanel *panel;
    if (gUnk_020004A4)
        return gUnk_020004A4;
    panel = Task_Create(9, 0x17E4);
    if (panel) {
        Task_SetCallbacks(panel, (void (*)(void *))sub_0805C8D8, (void (*)(void *))sub_0805CA20);
        ((struct ScenePanelCommandView *)panel)->actorId = actorId;
        if (sub_0805CA7C((void *)panel) < 0) {
            Task_Kill(panel);
            return NULL;
        }
    }
    return panel;
}
