#include "scene_panel_commands.h"
#define ScenePanel_CountActiveSubjects sub_0805C7B4
s32 sub_0805C7B4(struct ScenePanelInteractionView *panel)
{
    s32 active = 0;
    s32 remaining = panel->subjectCount;
    if (active < remaining) {
        struct ScenePanelInteractionSubject **subject = panel->subjects;
        do {
            if ((*subject)->state == 4)
                active++;
            subject++;
            remaining--;
        } while (remaining != 0);
    }
    return active;
}
