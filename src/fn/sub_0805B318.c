#include "scene_panel_interaction.h"
s32 ScenePanel_AreSubjectsSettled(struct ScenePanel *input)
{
    struct ScenePanelInteractionView *panel = (void *)input;
    s32 i = 0;
    s32 count = panel->subjectCount;
    if (i < count) {
        s32 limit = count;
        struct ScenePanelInteractionSubject **subject = panel->subjects;
        do {
            if ((*subject)->state != 2)
                return 0;
            subject++;
            i++;
        } while (i < limit);
    }
    return 1;
}
