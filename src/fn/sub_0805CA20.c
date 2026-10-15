#include "scene_panel_lifecycle.h"
#define ScenePanel_DestroyLifecycle sub_0805CA20
s32 sub_0805CA20(struct ScenePanelLifecycleView *parent){
 if(parent->initialMode==0){if(parent->paletteSprite.active)Sprite_Remove(&parent->paletteSprite);}
 else{ScenePanelRig_RemoveSprites(&parent->rig);Sprite_Remove(&parent->auxiliary);sub_08215A74(1,1);}
 ScenePanel_DestroyParticles((void*)parent);sub_0805C728((void*)parent);gUnk_020004A4=0;return 0;
}
