#include "scene_panel_interaction.h"
// MATCHING_CONCESSION: register-pinning

#define ScenePanel_UpdateActiveSubjects sub_0805B70C
void ScenePanel_UpdateActiveSubjects(struct ScenePanel *input)
{
    struct ScenePanelInteractionView *panel = (void *)input;
    s32 active = 0;
    s32 i = 0;
    for (; i < panel->subjectCount; i++) {
        struct ScenePanelInteractionSubject **subject;
        u32 offset;
        register struct ScenePanelInteractionSubject **base asm("r1");
        offset = i * 4;
        base = panel->subjects;
        subject = (void *)((u8 *)base + offset);
        if ((*subject)->state == 4) {
            active++;
            if ((*subject)->changed) {
                Sound_PlaySong(294);
                sub_08058150((void *)panel, sub_080582B8);
                (*subject)->changed = 0;
            }
        }
    }
    if (!panel->allSubjectsNotified && active >= panel->subjectCount) {
        panel->allSubjectsNotified = 1;
        sub_08057978((void *)panel, 1);
    }
    if (!panel->presentationState)
        sub_08058150((void *)panel, sub_08058444);
    if (sub_0805B34C((void *)panel)) {
        sub_0815B0A8(SCENE_PANEL_PLAYER, &panel->approachPosition, 0, 4);
        sub_08057960((void *)panel, 5);
    }
}
