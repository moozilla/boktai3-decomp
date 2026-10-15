#include "scene_panel_commands.h"
s32 ScenePanel_CountActiveSubjects(struct ScenePanelInteractionView *panel)
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
