#include "scene_panel_orchestration.h"
void ScenePanel_InitializeInteraction(struct ScenePanel *input)
{
    struct ScenePanelInteractionView *panel = (void *)input;
    u32 mode = panel->initialMode;
    switch (mode) {
        case 0: {
            s32 option;
            if (ScenePanel_AreSubjectsIdle((void *)panel)) {
                ScenePanel_InitPaletteSprite((void *)panel);
                ScenePanel_SetEffectState((void *)panel, 0);
                return;
            }
            if (Script_SeekToKeyword('O'))
                option = Script_GetValue();
            else
                option = 0;
            if (option && gUnk_02000488) {
                s32 active;
                u32 count;
                u8 *tetherState = &gUnk_02000488->state;
                active = 0;
                *tetherState = 12;
                gUnk_02000488->position = panel->center;
                gUnk_02000488->heading = 0;
                count = panel->subjectCount;
                if (active < (s32)count) {
                    struct ScenePanelInteractionSubject **subject = panel->subjects;
                    s32 remaining = count;
                    do {
                        if ((*subject)->state == 4)
                            active = 1;
                        subject++;
                        remaining--;
                    } while (remaining != 0);
                }
                if (active) {
                    ScenePanel_SetEffectState((void *)panel, 4);
                    return;
                }
            } else
                ScenePanel_InitPaletteSprite((void *)panel);
            ScenePanel_SetEffectState((void *)panel, 3);
            break;
        }
        case 1: {
            ScenePanel_InitRig((void *)panel);
            ScenePanel_InitResourceConfiguration((void *)panel);
            gUnk_02000488->state = 12;
            gUnk_02000488->position = panel->center;
            gUnk_02000488->heading = 0;
            gUnk_02000488->unknown1B = mode;
            if (gUnk_02000488->kind == 6)
                panel->resourceScale = 80;
            else if (gUnk_02000488->kind == 7)
                panel->resourceScale = 120;
            else
                panel->resourceScale = 100;
            ScenePanel_InitBackground((void *)panel);
            ScenePanel_InitOverlay((void *)panel);
            ScenePanel_InitAuxiliarySprite((void *)panel);
            ScenePanel_SetEffectState((void *)panel, 8);
        }
    }
}
