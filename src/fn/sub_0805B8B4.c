#include "scene_panel_interaction.h"
// MATCHING_CONCESSION: register-pinning

#define ScenePanel_AdvanceSubjectDeparture sub_0805B8B4
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
        sub_08058150((void *)panel, sub_080582B8);
        sub_0822B058(4);
    } else if (frame == 90) {
        sub_08226508(16);
        sub_0815B28C(SCENE_PANEL_PLAYER, 0);
    } else if (frame > 105) {
        struct ScenePanelInteractionPlayer *player = SCENE_PANEL_PLAYER;
        if (!player->tetherMode) {
            sub_0815C99C(player);
            sub_08057960((void *)panel, 3);
            panel->allSubjectsNotified = 0;
            sub_08057978((void *)panel, 2);
        }
    }
}
