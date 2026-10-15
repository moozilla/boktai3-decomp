#include "scene_panel_lifecycle.h"
#define ScenePanel_InitializeLifecycle sub_0805CA7C
s32 sub_0805CA7C(struct ScenePanelLifecycleView *parent){
 s32 found=Script_SeekToKeyword('d');
 if(found)parent->skipMainSprite=Script_GetValue();else parent->skipMainSprite=found;
 {struct ScenePanelInteractionPlayer *player=SCENE_PANEL_PLAYER;parent->savedPlayer=player;}parent->savedTether=gUnk_02000488;
 sub_0805BFC8((void*)parent);sub_0805C454((void*)parent);
 if(Script_SeekToKeyword('n'))parent->subjectCount=Script_GetValue();else parent->subjectCount=4;
 sub_0805BDD0((void*)parent);sub_0805C310((void*)parent);ScenePanel_InitParticles((void*)parent);ScenePanel_InitTimePalette((void*)parent);sub_0805C054((void*)parent);sub_0805C73C((void*)parent);
 {s32 mode=gUnk_02000710->unk876;parent->mode=mode;}gUnk_020004A4=(void*)parent;return 0;
}
