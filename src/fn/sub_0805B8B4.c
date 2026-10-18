#include "scene_panel_interaction.h"

void ScenePanel_AdvanceSubjectDeparture(struct ScenePanel *input)
{
    struct ScenePanelInteractionView *panel = (void *)input;
    u32 frame;
    do {
    } while (0);
    frame = (u16)(++panel->frame);
    if (frame == 30) {
        s32 i = 0;
        for (; i < panel->subjectCount; i++) {
            struct ScenePanelInteractionSubject *subject = panel->subjects[i];
            if (subject->state == 4) {
                ScenePanelSubject_SetState((void *)subject, 5);
            }
        }
        ScenePanel_SetState((void *)panel, ScenePanel_BeginLongSequence);
        Sound_FadeOutChannel10(4);
    } else if (frame == 90) {
        Camera_FollowPlayerWithoutCompletion(16);
        PlayerCue_Motion7End(SCENE_PANEL_PLAYER, 0);
    } else if (frame > 105) {
        struct ScenePanelInteractionPlayer *player = SCENE_PANEL_PLAYER;
        if (!player->tetherMode) {
            PlayerCue_ReleaseControl(player);
            ScenePanel_SetEffectState((void *)panel, 3);
            panel->allSubjectsNotified = 0;
            ScenePanel_CallPendingScript((void *)panel, 2);
        }
    }
}
