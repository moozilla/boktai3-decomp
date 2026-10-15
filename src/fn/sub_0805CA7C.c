#include "scene_panel_lifecycle.h"
s32 ScenePanel_InitializeLifecycle(struct ScenePanelLifecycleView *parent)
{
    s32 found = Script_SeekToKeyword('d');
    if (found)
        parent->skipMainSprite = Script_GetValue();
    else
        parent->skipMainSprite = found;
    {
        struct ScenePanelInteractionPlayer *player = SCENE_PANEL_PLAYER;
        parent->savedPlayer = player;
    }
    parent->savedTether = gUnk_02000488;
    ScenePanel_InitAnchorPositions((void *)parent);
    ScenePanel_ReadInitialMode((void *)parent);
    if (Script_SeekToKeyword('n'))
        parent->subjectCount = Script_GetValue();
    else
        parent->subjectCount = 4;
    ScenePanel_CreateSubjects((void *)parent);
    ScenePanel_InitializeInteraction((void *)parent);
    ScenePanel_InitParticles((void *)parent);
    ScenePanel_InitTimePalette((void *)parent);
    ScenePanel_InitScriptProperty((void *)parent);
    ScenePanel_RegisterReceiver((void *)parent);
    {
        s32 mode = gUnk_02000710->unk876;
        parent->mode = mode;
    }
    gUnk_020004A4 = (void *)parent;
    return 0;
}
