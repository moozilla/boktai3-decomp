#include "scene_panel_lifecycle.h"
s32 ScenePanel_TickLifecycle(struct ScenePanelLifecycleView *parent)
{
    u32 playerKind;
    ScenePanel_DispatchCommands((void *)parent);
    ScenePanel_SelectMode((void *)parent);
    parent->activeCount = ScenePanel_CountActiveSubjects((void *)parent);
    playerKind = SCENE_PANEL_PLAYER->kindValue;
    if (playerKind == 4)
        return 0;
    if (parent->initialMode == 1 && parent->state != 14 && playerKind == 1 &&
        !gUnk_02000710->unk618) {
        ScenePanel_SetEffectState((void *)parent, 14);
        ScenePanelRig_SetState(&parent->rig, 9);
        ScenePanel_NoOpAction((void *)parent);
        ScenePanel_CallPendingScript((void *)parent, 8);
    }
    gUnk_08605ABC[parent->state]((void *)parent);
    if (parent->initialMode == 0)
        ScenePanel_UpdatePaletteState((void *)parent);
    else {
        ScenePanel_TickRig((void *)parent);
        ScenePanel_SubmitPlayerMidpoint((void *)parent);
        ScenePanel_DrawBackgroundLayers((void *)parent);
        ScenePanel_LoadModePalette((void *)parent);
        ScenePanel_UpdateApproachSprite((void *)parent);
        if (parent->activated) {
            if (!parent->skipMainSprite) {
                if (gUnk_02000710->unk618 == 0) {
                    if (parent->warningSounds[2])
                        Sound_PlaySong(parent->warningSounds[2]);
                } else {
                    if (parent->warningSounds[parent->warningStage])
                        Sound_PlaySong(parent->warningSounds[parent->warningStage]);
                }
                Sound_PlaySong(0x29B);
            }
            parent->activated = 0;
        }
    }
    /* The non-void callback preserves the original fall-through return ABI. */
    ScenePanel_UpdateParticles((void *)parent);
}
