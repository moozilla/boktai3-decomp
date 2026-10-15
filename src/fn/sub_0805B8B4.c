#include "scene_panel_interaction.h"
// MATCHING_CONCESSION: register-pinning

void ScenePanel_AdvanceSubjectDeparture(struct ScenePanel *input)
{
    struct ScenePanelInteractionView *panel = (void *)input;
    u32 frame;
    asm("" : "+r"(panel));
    frame = (u16)(++panel->frame);
    if (frame == 30) {
        s32 i = 0;
        for (; i < panel->subjectCount; i++) {
            struct ScenePanelInteractionSubject *subject = panel->subjects[i];
            if (subject->state == 4)
                sub_0805CB88((void *)subject, 5);
        }
        ScenePanel_SetState((void *)panel, ScenePanel_BeginLongSequence);
        sub_0822B058(4);
    } else if (frame == 90) {
        sub_08226508(16);
        sub_0815B28C(SCENE_PANEL_PLAYER, 0);
    } else if (frame > 105) {
        struct ScenePanelInteractionPlayer *player = SCENE_PANEL_PLAYER;
        if (!player->tetherMode) {
            sub_0815C99C(player);
            ScenePanel_SetEffectState((void *)panel, 3);
            panel->allSubjectsNotified = 0;
            ScenePanel_CallPendingScript((void *)panel, 2);
        }
    }
}
