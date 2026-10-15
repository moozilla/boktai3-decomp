#include "scene_panel_subject.h"
#define ScenePanel_Create sub_0805CB38
struct ScenePanel *ScenePanel_Create(u32 actorId)
{
    struct ScenePanel *panel;
    if (gUnk_020004A4)
        return gUnk_020004A4;
    panel = Task_Create(9, 0x17E4);
    if (panel) {
        Task_SetCallbacks(panel, (void (*)(void *))ScenePanel_TickLifecycle,
                          (void (*)(void *))ScenePanel_DestroyLifecycle);
        ((struct ScenePanelCommandView *)panel)->actorId = actorId;
        if (ScenePanel_InitializeLifecycle((void *)panel) < 0) {
            Task_Kill(panel);
            return NULL;
        }
    }
    return panel;
}
