#include "scene_panel_interaction.h"
// MATCHING_CONCESSION: register-pinning

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
                ScenePanel_SetState((void *)panel, ScenePanel_BeginLongSequence);
                (*subject)->changed = 0;
            }
        }
    }
    if (!panel->allSubjectsNotified && active >= panel->subjectCount) {
        panel->allSubjectsNotified = 1;
        ScenePanel_CallPendingScript((void *)panel, 1);
    }
    if (!panel->presentationState)
        ScenePanel_SetState((void *)panel, ScenePanel_BeginPulseSequence);
    if (ScenePanel_CanAcceptPlayer((void *)panel)) {
        sub_0815B0A8(SCENE_PANEL_PLAYER, &panel->approachPosition, 0, 4);
        ScenePanel_SetEffectState((void *)panel, 5);
    }
}
